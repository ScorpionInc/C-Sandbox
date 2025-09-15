#!/bin/bash

COLOR='\e[1;33m'
UPBAR='\e[1;53m'
RESET='\e[0m'

CMAKE_FILTER='(CMake|[.]cmake[:])'
GIT_FILTER='[.]git/'
UNITY_FILTER='unity(_internals[.](c|h)|-src/|[.](c|h))'
SCRIPT_FILENAME=$(basename "${0}")

BASE_FILTER="(${CMAKE_FILTER}|${GIT_FILTER}|${UNITY_FILTER}|${SCRIPT_FILENAME}:)"
C_LNG_MATCH='[.](c|h):'

printf "${COLOR}\n"
echo "################################################################################"
echo "#                          Start of BARR-C check script.                       #"
echo "# This script makes no guarantee that all matches are correct or inclusive.    #"
echo "# This is meant to work as an assistant while performing C static analysis.    #"
echo "################################################################################"
printf "${RESET}\n"

printf "${UPBAR}"
echo "Checking available tools"
printf "${RESET}"
if command -v grep &> /dev/null; then
	echo "Validated 'grep' is available."
else
	echo "Required tool: 'grep' was not found."
	echo "The common package name for installing it is: 'grep'."
	exit 1
fi

if command -v cut &> /dev/null; then
	echo "Validated 'cut' is available."
else
	echo "Required tool: 'cut' was not found."
	echo "The common package name for installing it is: 'cut'."
	exit 1
fi

if command -v pcregrep &> /dev/null; then
	echo "Validated 'pcregrep' is available."
else
	echo "Required tool: 'pcregrep' was not found."
	echo "The common package name for installing it is: 'pcregrep'."
	exit 1
fi
echo

printf "${UPBAR}"
echo "Checking Author Alias"
printf "${RESET}"
grep -n --binary-files=without-match --ignore-case -R -E '^\s+([*]|//)\s+Author: (\w\s*)+' |
grep -v -E "${BASE_FILTER}" |
cut -f1,2 -d':' | sort --unique
echo

printf "${UPBAR}"
echo "Checking for //Debug comments"
printf "${RESET}"
grep -n --binary-files=without-match --ignore-case -R -E "(//|/[*])\s*[!]?d[e]?b[u]?g(ging)?\s*" |
grep -v -E "${BASE_FILTER}" |
cut -f1,2 -d':' |
sort --unique
echo

printf "${UPBAR}"
echo "Checking for { inlining"
printf "${RESET}"
grep -n --binary-files=without-match -R -E '(if|while|for)[ ]*[(].*[)][ ]*[{]$' |
grep -v -E "${BASE_FILTER}" |
cut -f1,2 -d':' | sort --unique
echo

printf "${UPBAR}"
echo "Checking for common Yoda Errors"
printf "${RESET}"
grep -n --binary-files=without-match -R -E '.*(==|!=|[^<]<[^<]|[^>]>[^>]|[^<]<=|[^>]>=|\|\|)[ ]*(NULL|0[ulf]?|1[ulf]?|true|false|"[\]0"|'\''[\]0'\'')[^a-zA-Z0-9]' |
grep -v -E "${BASE_FILTER}" |
grep -E "${C_LNG_MATCH}" |
cut -f1,2 -d':' | sort --unique
echo

printf "${UPBAR}"
echo "Checking for pointer p_ prefix"
printf "${RESET}"
#-P ^\s*((const|volatile)\s+)?([a-zA-Z_]+[0-9]*[a-zA-Z_]*)[*]+\s+(const\s+([^p\s]|[p]+[^p_\s])|((?!const\s+)[^p\s]|[p]+[^p_\s]))[^*]+
grep -n --binary-files=without-match -R -P '^\s*((const|volatile)\s+)?([a-zA-Z_]+[0-9]*[a-zA-Z_]*)[*]+\s+(const\s+([^p\s]|[p]+[^p_\s])|((?!const\s+)[^p\s]|[p]+[^p_\s]))[^*]+' | grep -v -E '([(]|[)])' |
grep -v -E "${BASE_FILTER}" |
cut -f1,2 -d':' | sort --unique
echo

printf "${UPBAR}"
echo "Checking for type _t suffix"
printf "${RESET}"
grep -n --binary-files=without-match -R -P '^\s*typedef\s+' |
grep -v -E '[a-zA-Z]+[a-zA-Z0-9]*(_t)' |
grep -v -E "${BASE_FILTER}" |
cut -f1,2 -d':' | sort --unique
echo

printf "${UPBAR}"
echo "Checking for conditional defines"
printf "${RESET}"
grep -n --binary-files=without-match -R -E '(if|while)[ ]*[(].*[^ (*][(].*[)].*[)]' |
grep -v -E "${BASE_FILTER}" |
cut -f1,2 -d':' | sort --unique
echo

printf "${UPBAR}"
echo "Checking for conditionals without a comparison operator"
printf "${RESET}"
grep -n --binary-files=without-match -R -E '(if|while)[ ]*[(].*[)]' | grep -v -E '(if|while)[ ]*[(].*(==|!=|<[^<]|>[^>]|<=|>=|\|\|).*[)]' |
grep -v -E "${BASE_FILTER}" |
grep -E "${C_LNG_MATCH}" |
cut -f1,2 -d':' | sort --unique
echo

printf "${UPBAR}"
echo "Checking for variables less than 3 in length. (WIP)"
printf "${RESET}"
# WIP / TODO
grep -n --binary-files=without-match -R -P '((const|volatile)\s+)?([a-zA-Z*]+[a-zA-Z0-9*]*)\s+[a-zA-Z]([a-zA-Z0-9]\s|\s)' | grep -E '[.](c|h):' | grep -v -E '([0-9]:\s*[#]|[0-9]:\s+\*\s+[^\s]|.*(//|/\*|\*/))' |
grep -v -E "${BASE_FILTER}" |
grep -E "${C_LNG_MATCH}" |
cut -f1,2 -d':' | sort --unique
echo

printf "${UPBAR}"
echo "Checking for multiple returns"
printf "${RESET}"
pcregrep -n --binary-files=without-match -r -M '^(?!extern)\{([^}"]*return[( ][^}"]*[;][^}]*){2,}\}' . |
grep -v -E "${BASE_FILTER}" |
grep -E ".*[:][0-9]+[:]" |
cut -f1,2 -d':' | sort --unique
echo

printf "${UPBAR}"
echo "Checking Not explicit cast(s)"
printf "${RESET}"
grep -n --binary-files=without-match -R -E '= to(lower|upper)[(]' |
grep -v -E "${BASE_FILTER}" |
cut -f1,2 -d':' | sort --unique
echo

printf "${UPBAR}"
echo "Checking Uninitialized structs"
printf "${RESET}"
grep -n --binary-files=without-match -R '{}' |
grep -v setUp | grep -v tearDown |
grep -v -E "${BASE_FILTER}" |
grep -E "${C_LNG_MATCH}" |
cut -f1,2 -d':' | sort --unique
echo

printf "${UPBAR}"
echo "Checking BARR-C Line length"
printf "${RESET}"
grep -n --binary-files=without-match -R '.\{80\}' |
grep -v -E "${BASE_FILTER}" |
grep -E "${C_LNG_MATCH}" |
cut -f1,2 -d':' | sort --unique
echo

printf "${COLOR}"
echo "################################################################################"
echo "#                           End of BARR-C check script.                        #"
echo "################################################################################"
printf "${RESET}"
echo

