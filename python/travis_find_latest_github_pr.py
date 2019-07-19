#!/usr/bin/env python3
from github import Github
from github.GithubException import UnknownObjectException
import os
import sys
import traceback
import time

github = Github(os.environ['GITHUB_OAUTH_USER'], os.environ['GITHUB_OAUTH_TOKEN'])
repo = github.get_repo(os.environ['TRAVIS_REPO_SLUG'])
commit_id = os.environ['TRAVIS_COMMIT']

pr = None

def find_pr():
    all_pulls = [pr for pr in repo.get_pulls(state='closed', sort='updated')]
    all_pulls.reverse()
    for merged_pr in all_pulls:
        if merged_pr.merge_commit_sha == commit_id:
            return merged_pr
    return None
pr = find_pr()
with open('release.env', 'w') as release_env:
    release_env.write('RELEASES_NAME="{}"\n'.format(pr.title))
    release_env.write('RELEASES_RELEASE_NOTES_FILE="release_notes.md"\n')
with open('release_notes.md', 'w') as release_notes:
    release_notes.write(pr.body)

