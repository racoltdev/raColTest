git status
echo "Enter branch name to push all tracked changes to (blank for skip)"
read PUSH
if [ ! -z "$PUSH" ]; \
	then echo "Enter commit message ";\
	read MESSAGE;\
	# git add -u\
	# git commit $$MESSAGE\
	# git push origin $$PUSH\
fi
