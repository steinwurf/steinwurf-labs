Git workflow
------------
This document describes our git workflow for the Steinwurf projects.

We roughly follow the work flow described here:

* http://scottchacon.com/2011/08/31/github-flow.html
* http://reinh.com/blog/2009/03/02/a-git-workflow-for-agile-teams.html

Based on these resources we have devised the following guidelines:

#. Don't break master, make sure everything that you
   merge into master builds and passes all tests.

#. When changing something, create a descriptive branch off master
   (e.g. add-symbol-operations-stats)
   ::
     git checkout -b add-symbol-operations-stats

   For a branch dealing with an issue in the issue tracker, use
   the issue id to prefix the branch name e.g.
   ::
     git checkout -b 32-android-segfault-large-blocks


#. Commit to that branch and push your work to an upstream
   branch on the server with the same name[pro-git-ch3-5_].
   ::
    git push -u origin add-symbol-operations-stats
   
#. Go to buildbot and check that the code compiles. 
   ::
   http://buildbot.steinwurf.dk:12344

#. During your development, make sure that your branch stays up to date with the master
   ::
     git fetch
     git merge master     

#. Remember to update the NEWS file to indicate if your changes 
   should be categorized as MAJOR, MINOR or BUGFIX.

#. When you need feedback or you think the branch is ready
   for merging, open a Pull Request on github.

#. When the Pull Request has been approved and tested on the buildbot, 
   you can merge your branch into master (use the Green button on github).
   At this point, you should also delete your branch on the remote 
   (use the button for this on github).

.. _pro-git-ch3-5: http://progit.org/book/ch3-5.html


Keeping forked repository up-to-date
------------------------------------

If you have forked a repository and want to keep it up to date with the original repository, 
setup an upstream repository, e.g.  
::
  git remote add upstream git@github.com:steinwurf/kodo.git

Whenever you want to rebase on the upstream branch
::
  git pull --rebase <remote name> <branch name>


You can also simply pull the upstream changes into your master
::
  git pull upstream master

If you have local branches you can subsequently rebase those on the updated master.


Working on a remote branch
--------------------------

Sometimes somebody else creates a new branch as does some work. Now you 
would like to contribute. To do this you need to get a local working copy
of the branch. See which branches are available:
::
  git fetch --prune
  git branch -r

Checkout the one you would like to work on:
::
  git checkout --track origin/add-symbol-operations-stats

You should now have a local brach to which you can push and pull.


Releasing a new version
-----------------------
Versions are added according to the following scheme:

http://semver.org/

We create new releases using the git tagging mechanism.

1. Make sure everything is committed and tests are running
2. Update the NEWS.rst file to describe the changes since
   the previous version.
3. Bump the version number in the wscript file
4. Bump version number in doxygen, Doxyfile
5. Bump version number in sphinx, conf.py
6. Push the NEWS.rst and wscript files and check the buildbot
   that everything builds before creating the tag.
7. Create and push the tag e.g.:

::

  git tag -a 1.0.2-gtest_1_6_0 -m "version 1.0.2"
  git push --tags

Merging a pull-request
----------------------
One great way to get contributions is via the Github pull-request system. 
In Steinwurf we use the following approach to merge incoming pull-requests:

* If the pull-request is a pure documentation update, simply review the change
  and merge at will.
* If the pull request contains code (bug-fixes, new features, etc.) we have to 
  first ensure that the changes work on all tested platforms (this is done using
  our buildbot).

Let's imagine that a pull-request with id 20 has been sent at the Kodo library. We 
now go through the following steps.

1. Create a new branch `pull-request-20`:
   ::
     git checkout -b pull-request-20

2. Apply the patch:
   ::
     curl https://github.com/steinwurf/kodo/pull/20.patch | git am

3. Push the patch to Github:
   ::
     git push origin pull-request-20

4. Check the buildbot (http://176.28.49.184:12344/) that all platforms and compilers work.

5. If succesfull merge with master and delete the branch otherwise notify the author of the 
   patch and report the errors encounterd. 

The above steps was inspired from here (http://beust.com/weblog/2010/09/15/a-quick-guide-to-pull-requests/)
