for loop in $(find -type d -empty)
do
echo "avoid empty directory" > $loop/.hidded
done
