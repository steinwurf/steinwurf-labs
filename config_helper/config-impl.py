#!/usr/bin/env python
# encoding: utf-8

import os
import sys
import json

try:
    input = raw_input
except NameError:
    pass

"""
  User specific variables
  NOTE: Changes to these should be done in a user-specific file called
        "fabric_user_config.py" located in the current directory(.) or
        in the parent directory(..)
"""
# There are no good default values for these
android_sdk_dir = None
android_ndk_dir = None
waf_projects = {}
resolve_path = None

try:
    # Also search in the parent folder when loading user config
    sys.path.append('..')
    from fabric_user_config import *
except Exception as e:
    pass


# Python recipe here: http://code.activestate.com/recipes/577058/
def query_yes_no(question, default="yes"):
    """Ask a yes/no question via input() and return their answer.

    "question" is a string that is presented to the user.
    "default" is the presumed answer if the user just hits <Enter>.
        It must be "yes" (the default), "no" or None (meaning
        an answer is required of the user).

    The "answer" return value is one of "yes" or "no".
    """
    valid = {"yes": "yes",   "y": "yes",  "ye": "yes",
             "no": "no",     "n": "no"}
    if default is None:
        prompt = " [y/n] "
    elif default == "yes":
        prompt = " [Y/n] "
    elif default == "no":
        prompt = " [y/N] "
    else:
        raise ValueError("invalid default answer: '%s'" % default)

    while 1:
        sys.stdout.write(question + prompt)
        choice = input().lower()
        if default is not None and choice == '':
            return default
        elif choice in valid.keys():
            return valid[choice]
        else:
            sys.stdout.write("Please respond with 'yes' or 'no' "
                             "(or 'y' or 'n').\n")


def query(question, default=None):
    prompt = ": "
    if default is not None:
        prompt = ' ["{}"]: '.format(default)
    sys.stdout.write(question + prompt)
    answer = input()
    if default is not None and answer == '':
        return default
    return answer


# Draw all the options, but highlight the selected index
def print_menu(options, question, default_index=0, multiple=False):
    counter = 0
    for idx, item in enumerate(options):
        print('  [{}] {}'.format(idx, item))
        counter += 1
    prompt = ' [{}] '.format(default_index)
    while 1:
        sys.stdout.write(question + prompt)
        choice = input().lower()
        if default_index is not None and choice == '':
            return options[default_index]
        elif multiple:
            indices = [int(i) for i in choice.split(',')
                       if i.isdigit() and int(i) < len(options)]
            return [options[i] for i in indices]
        elif choice.isdigit() and int(choice) < len(options):
            return options[int(choice)]
        else:
            sys.stdout.write("Please respond with a valid index!\n")

# Define some common build options
build_options = ['None', 'cxx_debug', 'cxx_nodebug']

# Define the supported mkspecs
android_mkspec = ['cxx_android_clang70_armv7', 'cxx_android5_clang70_armv7',
                  'cxx_android5_clang70_arm64']
msvc_mkspec = ['cxx_msvc15_x86', 'cxx_msvc15_x64']
gxx_mkspec = ['cxx_gxx52_x86', 'cxx_gxx52_x64',
              'cxx_gxx53_x86', 'cxx_gxx53_x64',
              'cxx_gxx54_x86', 'cxx_gxx54_x64',
              'cxx_gxx61_x86', 'cxx_gxx61_x64',
              'cxx_gxx62_x86', 'cxx_gxx62_x64',
              'cxx_gxx63_x86', 'cxx_gxx63_x64',
              'cxx_gxx73_x86', 'cxx_gxx73_x64',
              'cxx_gxx81_x86', 'cxx_gxx81_x64',
              'cxx_gxx82_x86', 'cxx_gxx82_x64']
cross_mskpec = ['cxx_openwrt_gxx53_arm', 'cxx_openwrt_gxx53_mips',
                'cxx_gxx63_armv7']
clang_mkspec = ['cxx_clang50_x86', 'cxx_clang50_x64',
                'cxx_clang60_x86', 'cxx_clang60_x64',
                'cxx_clang_address_sanitizer_x64',
                'cxx_clang_memory_sanitizer_x64',
                'cxx_clang_thread_sanitizer_x64']
llvm_mkspec = ['cxx_apple_llvm100_x64']
ios_mkspec = ['cxx_ios70_apple_llvm_armv7',
              'cxx_ios70_apple_llvm_armv7s',
              'cxx_ios70_apple_llvm_arm64',
              'cxx_ios70_apple_llvm_i386',
              'cxx_ios70_apple_llvm_x86_64']

# Define which mkspecs are supported on different platforms
win32_mkspec = msvc_mkspec + android_mkspec
linux_mkspec = gxx_mkspec + clang_mkspec + android_mkspec + cross_mskpec
mac_mkspec = llvm_mkspec + android_mkspec + ios_mkspec

# Project generator targets
project_targets = ['None', 'Visual Studio 2012', 'Visual Studio 2017']

def fetch_project_dependencies(project):

    base_url = "http://files.steinwurf.com/registry/dependencies"

    # Import tools to be compatible with Python 2 and 3
    try:
        from urllib.request import urlopen, Request
    except ImportError:
        from urllib2 import urlopen, Request

    try:
        # Fetch the json file from the given url
        url = '{}/{}.json'.format(base_url, project)
        req = Request(url)
        response = urlopen(req)
        json_data = response.read()
        data = json.loads(json_data)
        if 'dependencies' in data:
            return data["dependencies"].keys()
    except Exception as e:
        print("Could not fetch dependency info from:\n\t{}".format(url))
        print(e)

    return None


def config_options(available_mkspecs, dependencies=None, current_project=None):
    # Select the mkspec first
    print('\nSelect mkspec for {}:'.format(sys.platform))
    mkspec = print_menu(
        ['cxx_default'] + available_mkspecs, 'Choose option:', 0)
    print('Selected mkspec: ' + mkspec)
    tool_opt = '--cxx_mkspec=' + mkspec

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
        if android_sdk_dir:
            tool_opt += ' --android_sdk_dir=' + android_sdk_dir
        if android_ndk_dir:
            tool_opt += ' --android_ndk_dir=' + android_ndk_dir

    # Select the mkspec first
    print('\nSelect additional build options:')
    extra_option = print_menu(build_options, 'Choose option:', 0)
    print('Selected build option: ' + extra_option)
    if extra_option != 'None':
        tool_opt += ' --' + extra_option

    # Offer to generate project files for supported IDEs
    print('\nGenerate project files for the following IDEs?:')
    ide_names = \
        print_menu(project_targets, 'Choose options (e.g. "1,2,3"):', 0, True)
    print('Selected options: {}'.format(ide_names))

    ide_opt = ''
    if 'None' not in ide_names:
        if 'Visual Studio 2012' in ide_names:
            ide_opt += ' msvs2012'
        if 'Visual Studio 2017' in ide_names:
            ide_opt += ' msvs2017'
        ide_opt = ide_opt.strip()

    # The default configure command
    command = 'python waf configure'

    # Resolve options
    resolve_opt = ''

    # Gather information about local projects
    # We expect that the other projects are next to the current folder
    this_directory = os.path.dirname(os.getcwd())
    for folder in os.listdir(this_directory):
        project_folder = os.path.join(this_directory, folder)
        if os.path.isdir(project_folder) and folder not in waf_projects:
            waf_projects[folder] = os.path.relpath(project_folder)

    projects = []

    # Try to fetch an up-to-date dependency list from the registry at
    # http://files.steinwurf.com/registry/dependencies/
    if current_project is not None:
        result = fetch_project_dependencies(current_project)
        if result:
            dependencies = result

    # Enumerate the project dependencies
    for proj_name in dependencies:
        path = None
        # Look for this project name in waf_projects
        if proj_name in waf_projects:
            path = waf_projects[proj_name]

        if path is not None and os.path.exists(path):
            projects.append(proj_name)

    if dependencies is None:
        # If the dependencies are unknown, show all available local projects
        for proj_name, proj_path in waf_projects.iteritems():
            if os.path.exists(proj_path):
                if os.getcwd() != os.path.abspath(proj_path):
                    projects.append(proj_name)

    projects.sort()
    selected_projects = []
    if len(projects) > 0:
        project_options = list(projects)
        # Only include the ALL option if the dependencies were specified
        if dependencies:
            project_options.insert(0, 'ALL')
        project_options.insert(0, 'None')
        print('\nThe following project folders were found on your computer.\n'
              'Which folders should be used as dependencies?:')
        selected_projects = \
            print_menu(project_options, 'Choose projects (e.g. "1,2,3"):', 0, True)

        if 'ALL' in selected_projects:
            # Use the intersection of all dependencies and the locally
            # available projects
            selected_projects = list(set(dependencies) & set(projects))

        print('Selected projects: {}'.format(selected_projects))

        if 'None' not in selected_projects:
            for proj_name in selected_projects:
                # Use relative project path
                rel_path = os.path.relpath(waf_projects[proj_name])
                resolve_opt += ' --{}_path="{}"'.format(proj_name, rel_path)

    # resolve_path is not needed if ALL dependencies are resolved manually
    if 'ALL' not in selected_projects:
        global resolve_path
        if resolve_path == None:
            resolve_path = query('\nEnter resolve path', '../deps')

        if resolve_path != '':
            resolve_opt += ' --resolve_path="{}"'.format(resolve_path)

    # Assemble the final configure command
    full_cmd = str.format(
        '{} {} {} {}', command, resolve_opt, tool_opt, ide_opt).strip()
    print('\nFULL CONFIGURE COMMAND:\n' + full_cmd)

    # Save the configure command in a .bat or .sh file
    if sys.platform == 'win32':
        with open('last_config.bat', 'w') as bat:
            bat.write(full_cmd + '\n')
            bat.write('pause\n')    # the terminal will stay open
    else:
        with open('last_config.sh', 'w') as bat:
            bat.write('#!/bin/sh' + '\n')  # Shebang line
            bat.write(full_cmd + '\n')
        # Set script permission to executable
        os.system('chmod +x "{}"'.format('last_config.sh'))
    # Run the configure command
    os.system(full_cmd)

program_title = """
---------------------------------------
| Steinwurf Smart Project Config Tool |
---------------------------------------
"""


def config_tool(dependencies=None, current_project=None):
    print(program_title)
    if sys.platform == 'win32':
        config_options(win32_mkspec, dependencies, current_project)
    elif sys.platform == 'darwin':
        config_options(mac_mkspec, dependencies, current_project)
    elif sys.platform.startswith('linux'):
        config_options(linux_mkspec, dependencies, current_project)
    else:
        print('Platform "{}" is not supported.'.format(sys.platform))

if __name__ == '__main__':
    config_tool()
    input('Press ENTER to exit...')
