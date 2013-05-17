#!/usr/bin/env python
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
# There are no good default values for these
android_sdk_dir = None
android_ndk_dir = None
ios_toolchain_dir = None
ios_sdk_dir = None
# By default, we expect that the other projects are next to current dir
project_path = '../'
waf_projects = {}

default_waf_projects = \
{
# Public repos:
    'fifi':              project_path+'fifi',
    'kodo':              project_path+'kodo',
    'sak':               project_path+'sak',
    'gauge':             project_path+'cxx-gauge',
    'boost':             project_path+'external-boost-light',
    'gtest':             project_path+'external-gtest',
    'waf':               project_path+'external-waf',
    'waf-tools':         project_path+'external-waf-tools',
    'steinwurf-labs':    project_path+'steinwurf-labs',
# Private repos:
    'beem':              project_path+'beem',
    'imp':               project_path+'imp',
    'steinwurf-private': project_path+'steinwurf-private',
    'photofeed_engine':  project_path+'photofeed_engine',
    'photofeed_android': project_path+'photofeed_android',
}

bundle_path = project_path+'deps'
waf_build_path = project_path+'external-waf\\waf'



try:
    # Also search in the parent folder when loading user config
    sys.path.append('..')
    from fabric_user_config import *
    user_config = True
except Exception as e:
    print("Exception when loading fabric_user_config.py:")
    print(e)
    waf_projects = default_waf_projects


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
def print_menu(options, question, default_index=0, multiple=False):
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
        elif multiple:
            indices = [int(i) for i in choice.split(',') if i.isdigit() and int(i) < len(options)]
            return [options[i] for i in indices]
        elif choice.isdigit() and int(choice) < len(options):
            return options[int(choice)]
        else:
            sys.stdout.write("Please respond with a valid index!\n")

# Define the available build variants
build_variants = ['Release', 'Debug']

# Define the supported mkspecs
android_mkspec = ['cxx_android_gxx46_arm']
msvc_mkspec    = ['cxx_msvc11_x86', 'cxx_msvc11_x64']
gxx_mkspec     = ['cxx_gxx46_x86', 'cxx_gxx46_x64',
                  'cxx_gxx47_x86', 'cxx_gxx47_x64']
clang_mkspec   = ['cxx_clang30_x86', 'cxx_clang30_x64']
llvm_mkspec    = ['cxx_apple_llvm42_x86', 'cxx_apple_llvm42_x64']
ios_apple_mkspec = ['cxx_ios50_apple_llvm42_armv7']
ios_clang_mkspec = ['cxx_ios50_clang32_armv7']

# Define which mkspecs are supported on different platforms
win32_mkspec = msvc_mkspec + gxx_mkspec + android_mkspec + ios_clang_mkspec
linux_mkspec = gxx_mkspec + clang_mkspec + android_mkspec + ios_clang_mkspec
mac_mkspec = llvm_mkspec + gxx_mkspec + android_mkspec + ios_apple_mkspec

# Project generator targets
project_targets = ['None', 'Visual Studio 2008', 'Visual Studio 2010', 'Visual Studio 2012']


def config_options(available_mkspecs, dependencies = None):
    # Select the mkspec first
    print('\nSelect mkspec for {}:'.format(sys.platform))
    mkspec = print_menu(['cxx_default']+available_mkspecs, 'Choose option:', 0)
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

    # Handle extra options for iOS
    if mkspec in [ios_apple_mkspec] or mkspec in [ios_clang_mkspec]:
        # Try to set ios_toolchain_dir and ios_sdk_dir here
        # These variables might have been already set in user_config
        global ios_toolchain_dir
        global ios_sdk_dir
        # The XCode default toolchain path will be used
        # if the ios_toolchain_dir variable was not set
        toolchain = '/Applications/Xcode.app/Contents/Developer/' \
                    'Toolchains/XcodeDefault.xctoolchain/usr/bin/'
        if ios_toolchain_dir == None:
            ios_toolchain_dir = query('Enter ios_toolchain_dir', toolchain)
        if ios_sdk_dir == None:
            ios_sdk_dir = query('Enter ios_sdk_dir')
        tool_opt += ',ios_toolchain_dir='+ios_toolchain_dir
        tool_opt += ',ios_sdk_dir='+ios_sdk_dir

    # Select the mkspec first
    print('\nSelect build variant:')
    variant = print_menu(build_variants, 'Choose option:', 0)
    print('Selected build variant: '+variant)
    if variant == 'Debug':
        tool_opt += ',cxx_debug'

    # Offer to generate project files for supported IDEs
    print('\nGenerate project files for the following IDEs?:')
    ide_names = print_menu(project_targets, 'Choose options (e.g. "1,2,3"):', 0, True)
    print('Selected options: {}'.format(ide_names))

    ide_opt = ''
    if 'None' not in ide_names:
        if 'Visual Studio 2008' in ide_names:
            ide_opt += ' msvs2008'
        if 'Visual Studio 2010' in ide_names:
            ide_opt += ' msvs2010'
        if 'Visual Studio 2012' in ide_names:
            ide_opt += ' msvs2012'
        ide_opt = ide_opt.strip()

    # The default configure command
    command = 'python waf configure'

    # Bundle options
    bundle_opt = '--bundle=ALL'

    # print('Current dir: '+os.getcwd())
    projects = []
    # Enumerate the project dependencies
    if dependencies != None:
        for proj_name in dependencies:
            path = None
            # Look in the user-specified waf_projects first
            if proj_name in waf_projects:
                path = waf_projects[proj_name]
            # Also try the default path in default_waf_projects
            elif proj_name in default_waf_projects:
                path = default_waf_projects[proj_name]
                # Store the default project path for later
                if os.path.exists(path):
                    waf_projects[proj_name] = path
            if path != None and os.path.exists(path):
                projects.append(proj_name)
    else:
        # If the dependencies were not specified then show all available projects
        for proj_name, proj_path in waf_projects.iteritems():
            if os.path.exists(proj_path) and os.getcwd() != os.path.abspath(proj_path):
                projects.append(proj_name)

    proj_names = []
    if len(projects) > 0:
        projects.sort()
        # Only include the ALL option if the dependencies were specified
        if dependencies != None:
            projects.insert(0, 'ALL')
        projects.insert(0, 'None')
        print('\nThe following projects were found on your computer.\n'
                'Which projects should be used to directly resolve bundle dependencies?:')
        proj_names = print_menu(projects, 'Choose projects (e.g. "1,2,3"):', 0, True)
        print('Selected projects: {}'.format(proj_names))
        if 'ALL' in proj_names:
            bundle_opt = ''  # No bundle needed
            for proj_name in dependencies:
                rel_path = os.path.relpath(waf_projects[proj_name])
                bundle_opt += ' --{}-path="{}"'.format(proj_name, rel_path)
        elif 'None' not in proj_names:
            for proj_name in proj_names: # ALL,-project
                bundle_opt += ',-' + proj_name
            for proj_name in proj_names: # Use relative project path
                rel_path = os.path.relpath(waf_projects[proj_name])
                bundle_opt += ' --{}-path="{}"'.format(proj_name, rel_path)

    # bundle_path is not needed if ALL dependencies are resolved manually
    if 'ALL' not in proj_names:
        global bundle_path
        if user_config:
            print('\nUsing bundle path from your user_config: {}'.format(bundle_path))
            bundle_opt += ' --bundle-path="{}"'.format(os.path.relpath(bundle_path))
        else:
            # default_bundle_path = './bundle_dependencies'
            bundle_path = query('\nEnter bundle path',bundle_path)
            if bundle_path != '':
                bundle_opt += ' --bundle-path="{}"'.format(bundle_path)

    # Assemble the final configure command
    full_cmd = str.format('{} {} {} {}',command, bundle_opt, tool_opt, ide_opt).strip()
    print('\nFULL CONFIGURE COMMAND:\n'+full_cmd)

    # Save the configure command in a .bat or .sh file
    if sys.platform == 'win32':
        with open('last_config.bat', 'w') as bat:
            bat.write(full_cmd+'\n')
            bat.write('pause')    # the terminal will stay open
    else:
        with open('last_config.sh', 'w') as bat:
            bat.write('#!/bin/sh'+'\n') # Shebang line
            bat.write(full_cmd+'\n')
            bat.write('echo "Press ENTER to continue..."\n')
            bat.write('read')    # the terminal will stay open
        # Set script permission to executable
        os.system('chmod +x "{}"'.format('last_config.sh'))
    # Run the configure command
    os.system(full_cmd)

program_title = """
---------------------------------------
| Steinwurf Smart Project Config Tool |
---------------------------------------
"""

def config_tool(dependencies = None):
    print(program_title)
    if sys.platform == 'win32':
        config_options(win32_mkspec, dependencies)
    elif sys.platform == 'darwin':
        config_options(mac_mkspec, dependencies)
    elif sys.platform.startswith('linux'):
        config_options(linux_mkspec, dependencies)
    else:
        print('Platform "{}" is not supported.'.format(sys.platform))

if __name__ == '__main__':
    config_tool()
    raw_input('Press ENTER to exit...')
