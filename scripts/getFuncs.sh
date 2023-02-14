nm -C -f sysv raColTest | awk -F\| '$3 ~ /T/ && $7 ~ /.text/ {print}'
