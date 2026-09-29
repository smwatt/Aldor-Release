IncSrcs="foamlib.as"
AXL0Srcs="
	lang.as machine.as basic.as foamcat.as tuple.as gener.as 
	boolean.as segment.as sinteger.as sfloat.as pointer.as char.as 
	parray.as array.as list.as langx.as string.as
"
AXL1Srcs="
	format.as partial.as oslow.as fname.as file.as opsys.as textwrit.as
"
OKFILES="
	README build.sh OkFiles.sh
	$IncSrcs $AXL0Srcs $AXL1Srcs
"

echo $OKFILES
