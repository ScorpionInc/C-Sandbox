#!/bin/bash
#
# How to use:
# Execute script in directory of templates.
# Templates are expected to end in .template and have a matching .type file.
# Templates will have each instance of #TEMPLATE_TYPE replaced by each line of the .type file.
# Results will be saved into a subdirectory called 'generated' as rootFileName.type.h
#
echo 'Generate C template(s) script is starting.'
for template in *.template; do
	tRoot="${template%.template}"
	tFile="$tRoot.type";
	if test -f "$tFile"; then
		echo "Identified Template file to build: $template"; #Debugging
		while IFS="" read -r p || [ -n "$p" ]; do
			if [ -z "$p" ]; then
				continue;
			fi
			printf 'Generating for type: %s\n' "$p"; #Debugging
			mkdir -p ./generated || break;
			rm -f -- "./generated/$tRoot.$p.h"
			cat $template | sed --expression='s/#TEMPLATE_TYPE/'"$p"'/g' > "./generated/$tRoot.$p.h";
		done < "$tFile"
	fi
done
echo Script Stopping.
