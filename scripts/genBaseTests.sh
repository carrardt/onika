for HFILE in `find include -name "*.h"`
do
	INCFILE=`echo $HFILE | sed "s/include\///g"`
	TESTDEF=`echo $INCFILE | sed "s/\//_/g" | sed "s/\.h/_TEST/g"`
	INCFILESLASH=`echo $INCFILE | sed "s/\//_SLASH_/g"`
	TESTFILE="src/Tests/$TESTDEF.cpp"
	if [ ! -e $TESTFILE ]
	then
		echo "$HFILE"
		cat $HFILE | sed s/\$/_NEWLINE_/g | tr -d "\n" | sed "s/^.*#ifdef $TESTDEF/#include \"$INCFILESLASH\"/g" \
			   | sed "s/#endif.*#endif//g" | sed "s/_NEWLINE_/\n/g" | sed s/_SLASH_/\\\//g \
			   > $TESTFILE
		cat $HFILE | sed s/\$/_NEWLINE_/g | tr -d "\n" | sed "s/#ifdef $TESTDEF.*#endif/#endif/g" | sed "s/_NEWLINE_/\n/g" \
			   > $HFILE.new
		mv -f $HFILE.new $HFILE
	fi
done

