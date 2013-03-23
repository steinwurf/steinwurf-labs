#!/usr/bin/env python
# encoding: utf-8

import os
import sys
sys.path.append('..')
#import plotAll
#reload(plotAll)

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

# Draw all the options, but highlight the selected index
def print_menu(options, question, default_index=0):
    counter = 0
    for idx,item in enumerate(options):
        print str.format('[{}] {}', idx, item)
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
    print '\nSelect mkspec for '+sys.platform+':'
    mkspec = print_menu(win32_mkspec, 'Choose index', 0)
    print('Selected mkspec: '+mkspec)

    print '\nGenerate Visual Studio solution?:'
    vsver = print_menu(msvs_targets, 'Choose index', 0)
    print('Selected version: '+vsver)

    command = 'python waf configure'
    # bundle_opt = '--bundle=ALL --bundle-path="../deps"'
    bundle_opt = \
    '--bundle=ALL,-waf-tools --waf-tools-path="../external-waf-tools" --bundle-path="../deps"'
    tool_opt = '--options=cxx_mkspec='+mkspec
    msvs_opt = ''
    if vsver == 'Visual Studio 2008':
        msvs_opt = 'msvs2008'
    elif vsver == 'Visual Studio 2010':
        msvs_opt = 'msvs2010'
    elif vsver == 'Visual Studio 2012':
        msvs_opt = 'msvs2012'
    full_cmd = str.format('{} {} {} {}',command, bundle_opt, tool_opt, msvs_opt).strip()
    print('Full cmd: '+full_cmd)
    os.system(full_cmd)


if __name__ == '__main__':
    print('Smart Project Config Tool')
    if sys.platform == 'win32':
        config_win32()
    else:
        print("OS not supported.")

    raw_input('Press ENTER to exit...')
