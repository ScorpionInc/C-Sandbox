#!/bin/bash

echo "Checking available tools"
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

echo "Checking Author Alias"
grep -n --binary-files=without-match --ignore-case -R 'Author: ScorpionInc' | grep -v -E '(build[.]sh:|[.]git/|unity(_internals)?[.](c|h))' | cut -f1,2 -d':' | sort --unique
echo

echo "Checking for { inlining"
grep -n --binary-files=without-match -R -E '(if|while|for)[ ]*[(].*[)][ ]*[{]$' | grep -v -E '(build[.]sh:|[.]git/|unity(_internals)?[.](c|h))' | cut -f1,2 -d':' | sort --unique
echo

echo "Checking for common Yoda Errors"
grep -n --binary-files=without-match -R -E '(if|while)[ ]?[(].*(==|!=|<[^<]|>[^>]|<=|>=|\|\|)[ ]*(NULL|0[ulf]?|1[ulf]?|true|false|"[\]0"|'\''[\]0'\'')[ ]*[)]' | grep -v -E '(build[.]sh:|[.]git/|unity(_internals)?[.](c|h):)' | grep -E '[.](c|h):' | cut -f1,2 -d':' | sort --unique
echo

echo "Checking for pointer p_ prefix"
#WIP
#-E ^\s*([a-zA-Z0-9]+\s+)*[a-zA-Z0-9]+[*]+\s+(const\s+)?[^p][^*]+
#-P ^\s*((const|volatile)\s+)?([a-zA-Z_]+[0-9]*[a-zA-Z_]*)[*]+\s+(const\s+([^p]|[p][^_])|((?!const\s+)[^p]|[p][^_]))[^*]+
#-P ^\s*((const|volatile)\s+)?([a-zA-Z_]+[0-9]*[a-zA-Z_]*)[*]+\s+(const\s+([^p]|[p]+[^p_])|((?!const\s+)[^p]|[p]+[^p_]))[^*]+
grep -n --binary-files=without-match -R -P '^\s*((const|volatile)\s+)?([a-zA-Z_]+[0-9]*[a-zA-Z_]*)[*]+\s+(const\s+([^p]|[p]+[^p_])|((?!const\s+)[^p]|[p]+[^p_]))[^*]+' | grep -v -E '([(]|[)])'| grep -v -E '(build[.]sh:|[.]git/|unity(_internals)?[.](c|h))' | sort --unique
echo

echo "Checking for conditional defines"
grep -n --binary-files=without-match -R -E '(if|while)[ ]*[(].*[^ (*][(].*[)].*[)]' | grep -v -E '(build[.]sh:|[.]git/|unity(_internals)?[.](c|h))' | cut -f1,2 -d':' | sort --unique
echo

echo "Checking for conditionals without a comparison operator"
grep -n --binary-files=without-match -R -E '(if|while)[ ]*[(].*[)]' | grep -v -E '(if|while)[ ]*[(].*(==|!=|<[^<]|>[^>]|<=|>=|\|\|).*[)]' | grep -v -E '(build[.]sh:|[.]git/|unity(_internals)?[.](c|h))' | grep -E '[.](c|h):' | cut -f1,2 -d':' | sort --unique
echo

echo "Checking for multiple returns"
pcregrep -n --binary-files=without-match -r -M '^(?!extern)\{([^}"]*return[( ][^}"]*[;][^}]*){2,}\}' . | grep -v -E '(build[.]sh:|[.]git/|unity(_internals)?[.](c|h))' | grep -E ".*[:][0-9]+[:]" | cut -f1,2 -d':' | sort --unique
echo

echo "Checking Not explicit cast(s)"
grep -n --binary-files=without-match -R '= toupper(' | grep -v -E '(build[.]sh:|[.]git/|unity(_internals)?[.](c|h))' | cut -f1,2 -d':' | sort --unique
grep -n --binary-files=without-match -R '= tolower(' | grep -v -E '(build[.]sh:|[.]git/|unity(_internals)?[.](c|h))' | cut -f1,2 -d':' | sort --unique
echo

echo "Checking Uninitialized structs"
grep -n --binary-files=without-match -R '{}' | grep -E '[.](c|h):' | grep -v -E '(build[.]sh:|[.]git/|unity(_internals)?[.](c|h))' | grep -v setUp | grep -v tearDown | cut -f1,2 -d':' | sort --unique
echo

echo "Checking BARR-C Line length"
grep -n --binary-files=without-match -R '.\{80\}' | grep -E '[.](c|h):' | grep -v -E '(build[.]sh:|[.]git/|unity(_internals)?[.](c|h))' | cut -f1,2 -d':' | sort --unique
echo
