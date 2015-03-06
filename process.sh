# Get the last column
#cat avoidence.txt | grep 329462 | awk  '{print $NF}'

#Get the matrix values
cat avoidence.txt | grep 329462 | rev |cut -d' ' -f2- | rev

 
