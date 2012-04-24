Git work flow
-------------
This document describes our git work flow within the Steinwurf projects.

We roughly follow the work flow described here:

* http://scottchacon.com/2011/08/31/github-flow.html
* http://reinh.com/blog/2009/03/02/a-git-workflow-for-agile-teams.html

Based on these resources we have devised the following guidelines:

1. Don't break master, make sure everything you
   merge into master builds and passes all tests.

2. When changing something, create a descriptive branch off master
   (e.g. add-symbol-operations-stats)
   ::
     git checkout -b add-symbol-operations-stats

   For a branch dealing with an issue in the issue tracker, use
   the issue id to prefix the branch name e.g.
   ::
     git checkout -b 32-android-segfault-large-blocks


3. Commit to that branch and push your work to the same named
   branch on the server [pro-git-ch3-5_].
   ::
    git push -u origin add-symbol-operations-stats

4. Rebase against the upstream frequently to prevent your branch from
   diverging significantly
   ::
      git fetch origin master
      git rebase origin/master

   Notice, this is equivalent to
   ::
     git checkout master
     git pull
     git checkout add-symbol-operations-stats
     git rebase master

   But that method requires additional steps.


5. When you need feedback or help, or you think the branch is ready
   for merging open a pull request.

6. When someone else has reviews and signed off on the feature, you
   can merge it into master.
   ::
     git checkout master
     git merge add-symbol-operations-stats


7. Once it is merged - push it into master
   ::
     git push origin master
     git branch -d add-symbol-operations-stats




8. If you are done with your branch - remove it from the remote
   ::
     git push origin :add-symbol-operations-stats

   And delete your local branch
   ::
     git checkout master
     git branch -d add-symbol-operations-stats


.. _pro-git-ch3-5: http://progit.org/book/ch3-5.html


Keeping forked repository up to data
------------------------------------

If you have forked a repository and want to keep it up to date with the original repository, setup an upstream repository, e.g.
   
   ::
      git remote add upstream git@github.com:steinwurf/kodo.git


Whenever you want to pull in upstream changes into your master

   ::
      git pull upstream master

If you have local branches you can subsequently rebase those on the updated master.


Releasing a new version
-----------------------
Versions are added according to the following scheme:

http://semver.org/

We create new releases using the git tagging mechanism.

1. Make sure everything is committed and tests are running
2. Update the NEWS.rst file to describe the changes since
   the previous version.
3. Bump the version number in the wscript file.
4. Push the NEWS.rst and wscript files and check the buildbot
   that everything builds before creating the tag.
3. Create and push the tag e.g.:

::

  git tag -a 1.0.2-gtest_1_6_0 -m "version 1.0.2"
  git push --tags
