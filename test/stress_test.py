#!/usr/bin/env python3
import os
import time
import subprocess
import shutil
import datetime
import tempfile

def main():
    TMPROOT = r'/Volumes/RAM Disk'
    REPO = r"/Volumes/RAM Disk/redis/"
    SIT_BIN = r"/Users/abcdabcd987/Developer/sit/bin/sit"
    report = open('stress_test_%s.csv' % datetime.datetime.today().strftime('%Y%m%d_%H%M%S'), 'w')
    report.write('git_cur, sit_cur, git_tot, sit_tot, git_commit_cur, sit_commit_cur, git_commit_tot, sit_commit_tot, git_add_cur, sit_add_cur, git_add_tot, sit_add_tot\n')

    # get commits
    commits = ['unstable']
    args = ['git', 'log', '--pretty=%P', commits[0]]
    proc = subprocess.Popen(args, cwd=REPO, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    outs, errs = proc.communicate(timeout=15)
    for line in outs.decode('utf-8').split('\n'):
        last_commit = line.split(' ')[0]
        if not last_commit:
            break
        commits.append(last_commit)
    commits.reverse()

    # init repo
    tmp_sit = tempfile.mkdtemp(dir=TMPROOT)
    tmp_git = tempfile.mkdtemp(dir=TMPROOT)
    print('tmp_sit', tmp_sit)
    print('tmp_git', tmp_git)
    args = [SIT_BIN, 'init']
    proc = subprocess.Popen(args, cwd=tmp_sit)
    outs, errs = proc.communicate(timeout=15)
    args = ['git', 'init']
    proc = subprocess.Popen(args, cwd=tmp_git)
    outs, errs = proc.communicate(timeout=15)

    # write author config
    with open(os.path.join(tmp_sit, '.sit', 'config'), 'w') as f:
        f.write("user.name: Lequn Chen\nuser.email: i@abcdabcd987.com\n")

    timeused_sit = 0
    timeused_git = 0
    timeused_sit_commit = 0
    timeused_git_commit = 0
    timeused_sit_add = 0
    timeused_git_add = 0

    # loop commits
    for i, commit in enumerate(commits):
        cur_timeused_sit = 0
        cur_timeused_git = 0

        # delete old files
        for path in os.listdir(tmp_sit):
            if path != '.' and path != '..' and path != '.sit':
                p1 = os.path.join(tmp_sit, path)
                p2 = os.path.join(tmp_git, path)
                if os.path.isdir(p1):
                    shutil.rmtree(p1)
                    shutil.rmtree(p2)
                else:
                    os.remove(p1)
                    os.remove(p2)

        # checkout commit
        args = ['git', 'checkout', commit]
        proc = subprocess.Popen(args, cwd=REPO, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        outs, errs = proc.communicate(timeout=15)
        
        # copy new files
        for path in os.listdir(REPO):
            if path != '.' and path != '..' and path != '.git':
                src = os.path.join(REPO, path)
                dst1 = os.path.join(tmp_sit, path)
                dst2 = os.path.join(tmp_git, path)
                if os.path.isdir(src):
                    shutil.copytree(src, dst1)
                    shutil.copytree(src, dst2)
                else:
                    shutil.copy(src, dst1)
                    shutil.copy(src, dst2)

        # add
        time_st = time.time()
        args = [SIT_BIN, 'add', '.']
        proc = subprocess.Popen(args, cwd=tmp_sit, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        outs, errs = proc.communicate(timeout=15)        
        time_ed = time.time()
        cur_timeused_sit_add = time_ed - time_st

        time_st = time.time()
        args = ['git', 'add', '.']
        proc = subprocess.Popen(args, cwd=tmp_git, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        outs, errs = proc.communicate(timeout=15)        
        time_ed = time.time()
        cur_timeused_git_add = time_ed - time_st

        # get commit log
        args = ['git', 'log', '--pretty=%s', '-n', '1', commit]
        proc = subprocess.Popen(args, cwd=REPO, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        outs, errs = proc.communicate(timeout=15)
        message = outs.decode('utf-8')

        # write commit log
        with open(os.path.join(tmp_sit, '.sit', 'COMMIT_MSG'), 'w') as f:
            f.write(message)

        # commit
        time_st = time.time()
        args = [SIT_BIN, 'commit']
        proc = subprocess.Popen(args, cwd=tmp_sit, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        outs, errs = proc.communicate(timeout=15)
        time_ed = time.time()
        cur_timeused_sit_commit = time_ed - time_st

        time_st = time.time()
        args = ['git', 'commit', '-m', message]
        proc = subprocess.Popen(args, cwd=tmp_git, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        outs, errs = proc.communicate(timeout=15)
        time_ed = time.time()
        cur_timeused_git_commit = time_ed - time_st


        cur_timeused_sit = cur_timeused_sit_commit + cur_timeused_sit_add
        cur_timeused_git = cur_timeused_git_commit + cur_timeused_git_add
        timeused_sit += cur_timeused_sit
        timeused_git += cur_timeused_git
        timeused_sit_add += cur_timeused_sit_add
        timeused_git_add += cur_timeused_git_add
        timeused_sit_commit += cur_timeused_sit_commit
        timeused_git_commit += cur_timeused_git_commit

        print('[%d/%d] cur(%.0fms, %.0fms), tot(%.0fms, %.0fms) | cmt(%.0fms, %.0fms) | add(%.0fms, %.0fms)' % 
            (i, len(commits), 
             cur_timeused_git*1000, cur_timeused_sit*1000, 
             timeused_git*1000, timeused_sit*1000, 
             cur_timeused_git_commit*1000, cur_timeused_sit_commit*1000, 
             cur_timeused_git_add*1000, cur_timeused_sit_add*1000))
        report.write('%.10f, %.10f, %.10f, %.10f, %.10f, %.10f, %.10f, %.10f, %.10f, %.10f, %.10f, %.10f\n' % 
            (cur_timeused_git, cur_timeused_sit, timeused_git, timeused_sit, 
             cur_timeused_git_commit, cur_timeused_sit_commit, timeused_git_commit, timeused_sit_commit,
             cur_timeused_git_add, cur_timeused_sit_add, timeused_git_add, cur_timeused_sit_add))

    report.close()


main()
