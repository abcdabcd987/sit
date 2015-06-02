# 设计上的考量

## 树形结构 vs 列表列表

在设计 Commit 的文件记录的时候，我们最初的想法是直接使用和 `index` 一样的列表方式，即 `(sha1, path)` 二元组：

    87bbffd3a1c068111def6f38cf4e195aa08e9a37 utils/redis.conf.tpl
    af10d3a3c39b16d476945a2767f09d963c939f87 utils/redis-sha1.rb
    ee1a1142186d2f26d487dbb9297a4197926036b2 utils/install_server.sh
    164a227ff04074c4fbd86bcd6881641c739a41b8 utils/help.h
    237374c7b84f640b88742222eb161f2f2be566d3 utils/generate-command-help.rb
    6ddbed31efd3d6c5c2148dd77cade28aa3542d75 utils/build-static-symbols.tcl
    dcec114564d10da5bfc0e441c1cc13e6c3662ad6 tests/unit/type/zset.tcl
    11ee092a91aa9dc28095d6a4a7012920c93b79b4 tests/unit/type/set.tcl
    4d4a1185a19bdcb88a96bf5e5819032ebf4a0e2a tests/unit/type/list-common.tcl
    ac2e98b25e3e84c4371ae12f2821713cac92e7dc tests/unit/type/list-3.tcl

然而，在参考了 git 的实现之后，我们选择了使用树形结构来实现我们的文件记录。我们引入 `tree` 和 `blob` 两个概念。 `tree` 表示的就是文件系统中的一个目录，而 `blob` 表示的就是一个目录中的文件。这样，我们的文件列表即变为类似这样的树形结构，即 `(type, object id, filename)` 的多元组：

    tree e36c8aebd0c8d837f11af835ddf3c73d912abc2b src
    tree c266ec6cfd9b89b337feb285c3dd3a1d45a6f04e tests
    tree 9c9493aadc3d1faae7f0108d0465ccda9b3a0e6f utils
    blob f8beb85f2fd472896df6cbee3c88f37c4f91df75 zip.c
    blob f6f0ff0af6590f7ed077f889da5ea3748489994f ziplist.c
    blob 88af6ae3e023bc924afee7852b359f69155aca74 ziplist.h
    blob fa4b2ebfc54608bbf5f4c9642018879f317b4a97 zipmap.c
    blob 2c969f4a1e7784a545c3e733299a7b08308c8010 zipmap.h
    blob 1da2e22f30df5e329da57e41ced427603c8d8ebc zmalloc.c
    blob 935bbb5627be626dbbdba0f86ca3f9f362ec9ea7 zmalloc.h

可以做如下估计：若采用列表方式，假设项目中有 65536 个文件，又假设平均路径长度为 22 个字符，那么加上 SHA1 值及换行符之后，一行需要 64 个字符，也就是说，单次 commit 的文件列表需要占用 `65536 * 64 Bytes = 4 MBytes`。将以上假设应用于 linux 内核约 500000 次提交，则光是维护文件列表就需要占用 `2 Gbytes` 空间，这是一个相当大的浪费。

而采用树形结构来存储的话，每次提交只需要重建修改文件所在路径上所有父目录的 `tree` 对象。考虑到大型项目大致都有较为细分的目录结构，而且单次提交中修改的文件数量十分的少，因此在大型项目中的单次 commit 的文件列表占用空间就大大减小了。

## 处理树形结构与列表结构

虽然树形结构能节约很多的磁盘空间，然而在程序的处理过程中却并不容易，因此我们的做法是：在处理树形结构时先转换成内存中的列表结构，（如果有需要）再将列表结构转成树形结构写入磁盘。这么做虽然看起来比较浪费时间，但实际上每次运行 sit 时，这种转换的次数非常少。另外转换成列表后，程序的处理将变得更加简单和统一。

在 sit 执行各种任务时，会遇上以下几种不同的文件列表：

- 树形结构
  - repo 目录的文件系统
  - commit 的文件列表
- 列表结构
  - stage 区的 index

sit 的各种任务比如 `diff`, `status`, `checkout`, `reset` 等，需要同时处理以上多种列表。为了方便实现和减少重复代码，我们使用了 C++ 继承特性。

我们首先建立了基类 `IndexBase` 维护文件列表，支持对文件列表的查增删改。然后针对以上三种不同的文件列表，我们派生出了 `WorkingIndex`, `CommitIndex` 和 `Index` 这三个类，各自使用不同的方法读取列表及将列表写入文件。

这样一来，在 `diff` 等任务在处理时，无需关心当前处理的是哪一种列表，只需要向函数中传入 `IndexBase` 类型的变量即可，大大减少了重复代码以及繁冗的判断。

## diff 算法

对两个文件执行 diff 操作，我们的实现方法是先将文件按行执行 hash，这样就得到了两个序列。于是就可以将两个文件执行 diff 操作转换为求两个序列的最长公共子序列 (LCS)。

在按行 hash 时，我们最先尝试使用 `std::hash`，该函数能将 `std::string` hash 成为一个 `size_t` 值，并且具有非常高的效率。然而我们随后发现，假设某个文件有 50000 行文字，那么在一台32位的机器上对这 50000 个字符串进行 hash，由生日攻击的方法可以算出冲突概率（假设hash结果均匀分布）：

$$
100\%-\prod _{i=1}^{50000} \left(1-\frac{i-1}{2^{32}}\right) = 25.3 \%
$$

这是一个很糟糕的结果。而如果 hash 值提升到64位以上的话，在 50000 个字符串中冲突的概率几乎为0.

在得知 `std::hash` 使用了 `MurmurHash2` 算法后，最终我们选定了128位版本的 `MurmurHash3` 算法，这比32位的 `MurmurHash2` 算法拥有更高的效率和更低的冲突率。

在解决了 hash 问题后，重点就放在了解决 LCS 问题了。LCS 朴素的动态规划方法需要 `O(N*M)` 的时间及空间复杂度，其中 `N, M` 分别为两个字符串的长度。这并不是一个优秀的算法，时间复杂度和空间复杂度均不可接受。

经了解，`GNU diff` 和 `libxdiff` 这两个常用的 diff 工具所使用的 diff 方法都是基于 Eugene Myers 的论文 *An O(ND) Difference Algorithm and its Variations*。该算法期望时间复杂度为 `O(N+M+D^2)` ，最坏时间复杂度为 `O((N+M)D)` （可进一步优化至理论复杂度 `O((N+M)log(N+M) + D^2)`，其中 `D` 为两个序列的差异大小），并且具有线性的空间复杂度。

我们实现了这个算法。因为在sit的应用场景中，同一个文件的两个版本差异并不会很大，因此这个 `O((N+M)D)` 的算法获得了非常好的效果。

而 diff 算法还有许多进一步的优化空间，如：

- 在差异过大的时候可以使用近似算法以保证在极短的时间内完成比较
- LCS答案并不唯一，选择较优的方案输出

这两者在常见的 diff 程序中均有做出相应的优化，鉴于这些优化较为复杂，而且本次项目的重点并不在此，所以我们略过了这些优化。

## 彩色输出

Windows 下和 Unix 及 Linux 下的控制台彩色输出有所不同。在 Unix 和 Linux 中，可以直接向终端输出 ANSI 彩色序列来改变色彩，而在 Windows 中必须调用 Windows API 来实现。为了方便不同平台的彩色输出，我们重载了 `ostream` 的 `<<` 操作符，使得彩色输出变得异常的简便，如：

    cout << Color::RED << "Red" << Color::RESET << endl;