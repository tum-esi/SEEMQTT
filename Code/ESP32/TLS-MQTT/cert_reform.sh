#!/bin/bash

# This script reforms the input file and updates the variable in arduino project

# 3 input parameters:
#   parameter1: file that needs to be reformed
#   parameter2: variable that needs to be updated in arduinio project (ca_cert, client_cert or client_private_key)
#   parameter3: arduino project that needs to be updated
# 
#   e.g. sudo ./cert_reform.sh CA/ca.key ca_cert arduino_project.ino

echo "File before reforming:"
cat $1

echo ""
echo "File after reforming:"
sed 's@^@"@g' $1 | sed 's@$@\\n" \\@g' | sed '$ s@ \\$@;@' | tee reform.log

# Test if given variable already has value in arduino project
grep -A 1 "const.*$2" $3 | grep "\-\-\-*BEGIN"
result=$?

if [[ $result -eq 0 ]]; then
  # Find and delete the target variable value in source file
  sed -i "/$2/,/--*END/{/^\"/d}" $3
fi

# Update the target variable in source file
line_number=$( grep -n "const.*$2" $3 | cut -d ":" -f 1 )
sed -i "$line_number r reform.log" $3

rm reform.log
