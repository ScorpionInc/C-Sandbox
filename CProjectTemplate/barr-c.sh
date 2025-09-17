#!/bin/bash

# Function that tests if a value is in an array.
array_has_value()
{
	local received_array=("${@}")
	local received_len=${#received_array[@]}
	local needle=${received_array[received_len - 1]}
	local args=("${received_array[@]:0:${received_len}-1}")

	if [[ "${#}" -le "0" ]]; then
		echo "has_flag() Is missing required positional parameter(s)."
		echo "Usage:"
		echo -e "\tarray_has_value (\"Array\" \"Value\") \"Value\""
		return 0
	elif [[ "${#}" -le "1" ]]; then
		# This may due to an empty array.
		return 0
	fi

	for arg in "${args[@]}"; do
		if [[ "${arg}" == "${needle}" ]]; then
			return 1
		else
			:;
		fi
	done
	return 0
}

# ANSI Codes
array_has_value "${@}" "--no-ansi" && CLR_RED='\e[1;31m' || CLR_RED=''
array_has_value "${@}" "--no-ansi" && CLR_GRN='\e[1;32m' || CLR_GRN=''
array_has_value "${@}" "--no-ansi" && CLR_YLW='\e[1;33m' || CLR_YLW=''
array_has_value "${@}" "--no-ansi" && CLR_CYN='\e[1;36m' || CLR_CYN=''

array_has_value "${@}" "--no-ansi" && UPBAR='\e[1;53m' || UPBAR=''
array_has_value "${@}" "--no-ansi" && RESET='\e[0m' || RESET=''

# Global Constants
CMAKE_FILTER='(CMake|[.]cmake[:])'
GIT_FILTER='[.]git/'
UNITY_FILTER='unity(_internals[.](c|h)|-src/|[.](c|h))'
SCRIPT_FILENAME=$(basename "${0}")

BASE_FILTER="(${CMAKE_FILTER}|${GIT_FILTER}|${UNITY_FILTER}|${SCRIPT_FILENAME}:)"
C_LNG_MATCH='[.](c|h):'

# Functions use globals
validate_tool()
{
	if [[ "${#}" -le "0" ]]; then
		printf "${CLR_RED}"
		echo "validate_tool() Is missing required positional parameter: package(\$1)."
		echo "Usage:"
		echo -e "\tvalidate_tool \"find\""
		printf "${RESET}"
		return
	fi
	if command -v "${1}" &> /dev/null; then
		printf "${CLR_GRN}"
		echo "Validated '${1}' is available."
		printf "${RESET}"
	else
		printf "${CLR_RED}"
		echo "Required tool: '${1}' was not found."
		if command -v "apt-cache" &> /dev/null; then
			echo "Checking for possible package names for installing it. (Ctrl+C to break):"
			apt-config search ".*${1}.*" | grep -P "^(?!${1})*[a-zA-Z0-9]*(${1})[^-]*[-]"
		fi
		printf "${RESET}"
		exit 1
	fi
}

# Handles a single type of error check/test.
check_error()
{
	local req_args="3"

	if [[ "${#}" -lt "${req_args}" ]]; then
		printf "${CLR_RED}"
		echo "check_error() Is missing the required number of positional parameters. ${#}/${req_args}"
		printf "${RESET}"
		echo "Usage:"
		echo -e "\tcheck_error \"Short description\" \"Long Description\" \"\$Results\" [optional]"
		return
	fi

	if [ -n "${3}" ]; then
		printf "${UPBAR}"
		if [ -n "${1}" ]; then
			echo "Checking for ${1}"
		else
			echo "Checking for an unspecified error"
		fi
		printf "${RESET}"
		if [ -n "${2}" ]; then
			array_has_value "${@}" "-v" || echo -e "\t${2}"
		fi
		printf "${CLR_RED}"
		array_has_value "${@}" "-o" && (echo "${3}" | cut -f1,2 -d':') || echo "${3}"
		printf "${RESET}"
		echo
	fi
}

# Handles all checks that would be errors or violations of the BARR-C standard.
check_errors()
{
	local check=""

	printf "${CLR_RED}"
	echo "################################################################################"
	echo "#                          Error Level Tests/Checks                            #"
	echo "################################################################################"
	printf "${RESET}"
	echo

	check=$(
		grep -n --binary-files=without-match -R -E '(if|while|for)[ ]*[(].*[)][ ]*[{]$' |
    	grep -v -E "${BASE_FILTER}" |
		sort --unique
	)
	check_error\
		"{ inlining"\
		"If a brace is inline. E.G. if(conditional == condition){"\
		"${check}" "${@}"

	check=$(
		grep -n --binary-files=without-match -R -E '.*(==|!=|[^<]<[^<]|[^>]>[^>]|[^<]<=|[^>]>=|\|\|)[ ]*(NULL|0[ulf]?|1[ulf]?|true|false|"[\]0"|'\''[\]0'\'')[^a-zA-Z0-9]' |
		grep -v -E "${BASE_FILTER}" |
		grep -E "${C_LNG_MATCH}" |
		sort --unique
	)
	check_error\
		"common Yoda errors"\
		"'Left side the constants are.' ~Yoda (probably)"\
		"${check}" "${@}"

	check=$(
		grep -n --binary-files=without-match -R -E '^\s*auto\s+' |
		grep -v -E "${BASE_FILTER}" |
		grep -E "${C_LNG_MATCH}" |
		sort --unique
	)
	check_error\
		"forbidden keyword: 'auto'"\
		"The auto keyword is an unnecessary historical feature of C and forbidden by BARR-C."\
		"${check}" "${@}"

	check=$(
		grep -n --binary-files=without-match -R -E '^\s*register\s+' |
		grep -v -E "${BASE_FILTER}" |
		grep -E "${C_LNG_MATCH}" |
		sort --unique
	)
	check_error\
		"forbidden keyword: 'register'"\
		"The register keyword presumes a programmer is smarter than a compiler & is forbidden."\
		"${check}" "${@}"

	check=$(
		grep -n --binary-files=without-match -R -P '^\s*((const|volatile)\s+)?([a-zA-Z_]+[0-9]*[a-zA-Z_]*)[*]+\s+(const\s+([^p\s]|[p]+[^p_\s])|((?!const\s+)[^p\s]|[p]+[^p_\s]))[^*]+' |
		grep -v -E '([(]|[)])' |
		grep -v -E "${BASE_FILTER}" |
		sort --unique
	)
	check_error\
		"pointer p_ prefixes"\
		"Pointer type variable prefix. E.G. char* p_str; not char* str;"\
		"${check}" "${@}"

	check=$(
		grep -n --binary-files=without-match -R -P '^\s*typedef\s+struct\s+' |
		grep -v -E '[a-zA-Z]+[a-zA-Z0-9]*(_t)' |
		grep -v -E "${BASE_FILTER}" |
		sort --unique
	)
	check_error\
		"typedef struct _t suffixes"\
		"typedef struct suffix. E.G. typedef struct obj_t; typedef struct obj;"\
		"${check}" "${@}"

	check=$(
		grep -n --binary-files=without-match -R -E '(if|while)[ ]*[(].*[^ (*!][(].*[)].*[)]' |
		grep -v -E "${BASE_FILTER}" |
		sort --unique
	)
	check_error\
		"conditional defines"\
		"Tests for not allowed case of setting values inside of a conditional."\
		"${check}" "${@}"

	check=$(
		grep -n --binary-files=without-match -R -P '\s+[a-zA-Z]([a-zA-Z0-9]\s|\s)[=]\s' |
		grep -v -E "${BASE_FILTER}" |
		grep -E "${C_LNG_MATCH}" |
		sort --unique
	)
	check_error\
		"variables less than 3 in length."\
		"Lines where variables are declared with a name less than 3 in length."\
		"${check}" "${@}"

	check=$(
		pcregrep -n --binary-files=without-match -r -M '^(?!extern)\{([^}"]*return[( ][^}"]*[;][^}]*){2,}\}' . |
		grep -v -E "${BASE_FILTER}" |
		grep -E ".*[:][0-9]+[:]" |
		sort --unique
	)
	check_error\
		"multiple returns"\
		"Per BARR-C: Only one return statement per function."\
		"${check}" "${@}"

	check=$(
		grep -n --binary-files=without-match -R '{}' |
		grep -v setUp | grep -v tearDown |
		grep -v -E "${BASE_FILTER}" |
		grep -E "${C_LNG_MATCH}" |
		sort --unique
	)
	check_error\
		"uninitialized structs"\
		"Lines where variables are not being immediatly initialized."\
		"${check}" "${@}"

	check=$(
		grep -n --binary-files=without-match -R '.\{80\}' |
		grep -v -E "${BASE_FILTER}" |
		grep -E "${C_LNG_MATCH}" |
		sort --unique
	)
	check_error\
		"BARR-C Line length"\
		"Per BARR-C all lines must be less than or equal to 80 chars in length."\
		"${check}" "${@}"

	printf "${UPBAR}"
	echo "Checking for monolith function(s) over 100 lines (WIP/SLOW)"
	printf "${RESET}"
	array_has_value "${@}" "-v" ||
		echo -e "\tFunctions are to be less than 100 lines in length."
	#counter=0
	#top_line=""
	#functions=$(pcregrep -n --exclude="${BASE_FILTER}" --binary-files=without-match -r -M -o '(?s)\b\w+\s*\([^)]*\)\s*\{(.*?)\}\s*\n' . | grep -v -P '^\s*$')
	#while IFS= read -r line; do
	#	if [[ "${line}" == "}" ]]; then
	#		if [[ "$counter" -ge 100 ]]; then
	#			echo "${top_line} -> ${counter}"
	#		fi
	#		top_line=""
	#		counter=0
	#	else
	#		if [[ -z "$top_line" ]]; then
	#			top_line="${line}"
	#		fi
	#		((counter++))
	#	fi
	#done <<< "${functions}"
	printf "${RESET}"
	echo
}

# Handles a single check of potential violations or recommended code changes.
check_warning()
{
	local req_args="3"

	if [[ "${#}" -lt "${req_args}" ]]; then
		printf "${CLR_RED}"
		echo "check_warning() Is missing the required number of positional parameters. ${#}/${req_args}"
		printf "${RESET}"
		echo "Usage:"
		echo -e "\tcheck_warning \"Short describe\" \"Long Describe\" \"\$Results\" [optional]"
		return
	fi

	if [ -n "${3}" ]; then
		printf "${UPBAR}"
		if [ -n "${1}" ]; then
			echo "Checking for ${1}"
		else
			echo "Checking for an unspecified warning"
		fi
		printf "${RESET}"
		if [ -n "${2}" ]; then
			array_has_value "${@}" "-v" || echo -e "\t${2}"
		fi
		printf "${CLR_YLW}"
		array_has_value "${@}" "-o" && (echo "${3}" | cut -f1,2 -d':') || echo "${3}"
		printf "${RESET}"
		echo
	fi
}

# Handles all checks/tests for potential violations or recommended code changes
check_warnings()
{
	local check=""

	printf "${CLR_YLW}"
	echo "################################################################################"
	echo "#                         Warning Level Tests/Checks                           #"
	echo "################################################################################"
	printf "${RESET}"
	echo

	check=$(
		grep -n --binary-files=without-match --ignore-case -R -E '^\s+([*]|//)\s+Author: (\w\s*)+' |
		grep -v -E "${BASE_FILTER}" |
		sort --unique
	)
	check_warning\
		"Author Alias"\
		"This is a comment potentially declaring a code author."\
		"${check}" "${@}"

	check=$(
		grep -n --binary-files=without-match --ignore-case -R -E "(//|/[*])\s*[!]?d[e]?b[u]?g(ging)?\s*" |
		grep -v -E "${BASE_FILTER}" |
		sort --unique
	)
	check_warning\
		"//Debug comments"\
		"These are lines of code that are probably not meant to stay."\
		"${check}" "${@}"

	check=$(
		grep -n --binary-files=without-match -R -E '(if|while)[ ]*[(].*[)]' |
		grep -v -E '(if|while)[ ]*[(].*(==|!=|<[^<]|>[^>]|<=|>=|\|\|).*[)]' |
		grep -v -E "${BASE_FILTER}" |
		grep -E "${C_LNG_MATCH}" |
		sort --unique
	)
	check_warning\
		"conditionals without a comparison operator"\
		"E.G. wants if(true == condition) rather than if(condition)"\
		"${check}" "${@}"

	check=$(
		grep -n --binary-files=without-match -R -E '= to(lower|upper)[(]' |
		grep -v -E "${BASE_FILTER}" |
		sort --unique
	)
	check_warning\
		"non-explicit cast(s)"\
		"char* p_str = (char*)calloc(...); rather than char* p_str = calloc(...);"\
		"${check}" "${@}"

	check=$(
		grep -n --binary-files=without-match -R -E '\s+continue\s*[;]' |
		grep -v -E "${BASE_FILTER}" |
		grep -E "${C_LNG_MATCH}" |
		sort --unique
	)
	check_warning\
		"usage of limited keyword: 'continue'"\
		"Uses of the continue; keyword are to be limited per BARR-C."\
		"${check}" "${@}"
}

# Script main execution function
main()
{
	printf "${CLR_CYN}\n"
	echo "################################################################################"
	echo "#                          Start of BARR-C check script.                       #"
	echo "# This script makes no guarantee that all matches are correct or inclusive.    #"
	echo "# This is meant to work as an assistant while performing C static analysis.    #"
	echo "################################################################################"
	printf "${RESET}\n"

	echo "Valid script flags:"
	echo -e "\t--no-ansi - Disables ANSI colors and formatting in this script."
	echo -e "\t--all - Run all/warning tests. (Recommended)"
	echo -e "\t-v - Verbose adds description to each check/test being ran."
	echo -e "\t-o - Outputs the matching line after the file and line number."
	echo

	printf "${UPBAR}"
	echo "Checking available tools"
	printf "${RESET}"
	array_has_value "${@}" "-v" ||
		echo -e "\tThis is ensuring that the tools used are installed/available."
	TOOLS=("grep" "cut" "pcregrep");
	for tool in "${TOOLS[@]}"; do
		validate_tool "$tool"
	done
	echo

	check_errors "${@}"
	array_has_value "${@}" "--all" || check_warnings "${@}"

	printf "${CLR_CYN}"
	echo "################################################################################"
	echo "#                           End of BARR-C check script.                        #"
	echo "################################################################################"
	printf "${RESET}"
	echo
}

main "${@}"
