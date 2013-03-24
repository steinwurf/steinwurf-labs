﻿#!/usr/bin/env python
# encoding: utf-8

import os
import sys

user_config = False
"""
  User specific variables
  NOTE: Changes to these should be done in a user-specific file called
        "fabric_user_config.py" located in the current directory(.) or
        in the parent directory(..)
"""
android_sdk_dir = None
android_ndk_dir = None
waf_projects = {}
bundle_path = None
waf_build_path = None

try:
    # Also search in the parent folder when loading user config
    sys.path.append('..')
    from fabric_user_config import *
    user_config = True
except Exception as e:
    print("Exception when loading fabric_user_config.py:")
    print(e)


## Python recipe here: http://code.activestate.com/recipes/577058/
def query_yes_no(question, default="yes"):
    """Ask a yes/no question via raw_input() and return their answer.

    "question" is a string that is presented to the user.
    "default" is the presumed answer if the user just hits <Enter>.
        It must be "yes" (the default), "no" or None (meaning
        an answer is required of the user).

    The "answer" return value is one of "yes" or "no".
    """
    valid = {"yes":"yes",   "y":"yes",  "ye":"yes",
             "no":"no",     "n":"no"}
    if default == None:
        prompt = " [y/n] "
    elif default == "yes":
        prompt = " [Y/n] "
    elif default == "no":
        prompt = " [y/N] "
    else:
        raise ValueError("invalid default answer: '%s'" % default)

    while 1:
        sys.stdout.write(question + prompt)
        choice = raw_input().lower()
        if default is not None and choice == '':
            return default
        elif choice in valid.keys():
            return valid[choice]
        else:
            sys.stdout.write("Please respond with 'yes' or 'no' "\
                             "(or 'y' or 'n').\n")


def query(question, default=None):
    prompt = ": "
    if default is not None:
        prompt = str.format(' ["{}"]: ', default)
    sys.stdout.write(question + prompt)
    answer = raw_input()
    if default is not None and answer == '':
        return default
    return answer


# Draw all the options, but highlight the selected index
def print_menu(options, question, default_index=0):
    counter = 0
    for idx,item in enumerate(options):
        print str.format('  [{}] {}', idx, item)
        counter += 1
    prompt = str.format(' [{}] ', default_index)
    while 1:
        sys.stdout.write(question + prompt)
        choice = raw_input().lower()
        if default_index is not None and choice == '':
            return options[default_index]
        elif choice.isdigit() and int(choice) < len(options):
            return options[int(choice)]
        else:
            sys.stdout.write("Please respond with a valid index!\n")



android_mkspec = ['cxx_gxx46_arm_android']
win32_mkspec = ['cxx_msvc11_x86', 'cxx_msvc11_x64',
                'cxx_gxx46_x86', 'cxx_gxx47_x86'] + android_mkspec
posix_mkspec = ['cxx_gxx46_x86', 'cxx_gxx46_x64',
                'cxx_gxx47_x86', 'cxx_gxx47_x64'] + android_mkspec

msvs_targets = ['None', 'Visual Studio 2008', 'Visual Studio 2010', 'Visual Studio 2012']

def config_win32():
    # Select the mkspec first
    print('\nSelect mkspec for {}:'.format(sys.platform))
    mkspec = print_menu(win32_mkspec, 'Choose option:', 0)
    print('Selected mkspec: '+mkspec)
    tool_opt = '--options=cxx_mkspec='+mkspec

    # Handle extra options for Android
    if mkspec in android_mkspec:
        # Need to set android_sdk_dir and android_ndk_dir here
        # These variables might have been already set in user_config
        global android_sdk_dir
        global android_ndk_dir
        if android_sdk_dir == None:
            android_sdk_dir = query('Enter android_sdk_dir')
        if android_ndk_dir == None:
            android_ndk_dir = query('Enter android_ndk_dir')
        tool_opt += ',android_sdk_dir='+android_sdk_dir
        tool_opt += ',android_ndk_dir='+android_ndk_dir

    # Offer to generate project files for supported IDEs
    print('\nGenerate Visual Studio solution?:')
    vsver = print_menu(msvs_targets, 'Choose option:', 0)
    print('Selected option: '+vsver)

    msvs_opt = ''
    if vsver == 'Visual Studio 2008':
        msvs_opt = 'msvs2008'
    elif vsver == 'Visual Studio 2010':
        msvs_opt = 'msvs2010'
    elif vsver == 'Visual Studio 2012':
        msvs_opt = 'msvs2012'

    # The default configure command
    command = 'python waf configure'

    # Bundle options
    bundle_opt = '--bundle=ALL'
    if user_config:
        # print('Current dir: '+os.getcwd())
        projects = []
        for proj_name, proj_path in waf_projects.iteritems():
            if os.path.exists(proj_path) and os.getcwd() != os.path.abspath(proj_path):
                projects.append(proj_name)
        if len(projects) > 0:
            projects.sort()
            projects.insert(0, 'None')
            print('\nThe following projects were found in your user_config.\n'
                    'Which projects should be used to directly resolve bundle dependencies?:')
            proj_name = print_menu(projects, 'Choose projects:', 0)
            if proj_name != 'None':
                bundle_opt += ',-' + proj_name
                rel_path = os.path.relpath(waf_projects[proj_name])
                bundle_opt += ' --{}-path="{}"'.format(proj_name, rel_path)

        if bundle_path is not None:
            print('Using bundle path from your user_config: {}'.format(bundle_path))
            bundle_opt += ' --bundle-path="{}"'.format(os.path.relpath(bundle_path))
    else:
        default_bundle_path = './bundle_dependencies'
        dep_path = query('Enter bundle path to change default "{}"'.format(default_bundle_path))
        if dep_path != '':
            bundle_opt += ' --bundle-path="{}"'.format(dep_path)

##    bundle_opt = '--bundle=ALL,-waf-tools,-beem' \
##        ' --waf-tools-path="../external-waf-tools" --beem-path="../beem"' \
##        ' --bundle-path="../deps"'

    # Assemble the final configure command
    full_cmd = str.format('{} {} {} {}',command, bundle_opt, tool_opt, msvs_opt).strip()
    print('\nFULL CONFIGURE COMMAND:\n'+full_cmd)
    with open('last_config.bat', 'w') as bat:
        bat.write(full_cmd)
        bat.write('\npause')    # the terminal will stay open
    # Run the configure command
    os.system(full_cmd)

program_title = """
---------------------------------------
| Steinwurf Smart Project Config Tool |
---------------------------------------
"""

def config_tool():
    print(program_title)
    if sys.platform == 'win32':
        config_win32()
    else:
        print("OS not supported.")

if __name__ == '__main__':
    config_tool()
    raw_input('Press ENTER to exit...')
