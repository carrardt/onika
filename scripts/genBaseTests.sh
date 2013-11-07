for HFILE in `find include -name "*.h"`
do
	INCFILE=`echo $HFILE | sed "s/include\///g"`
	TESTDEF=`echo $INCFILE | sed "s/\//_/g" | sed "s/.h/_TEST/g"`
	echo "#define $TESTDEF 1" > src/Tests/$TESTDEF.cpp
	echo "#include \"$INCFILE\"" >> src/Tests/$TESTDEF.cpp
	echo "" >> src/Tests/$TESTDEF.cpp
done

