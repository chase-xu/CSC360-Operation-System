# CSC360-P3
Chase Xu, V00924503

For my assignment's parts are all without subdirectory implementation. Attention, my code is strict on parameters, so please no typo and wrong number of parameters, otherwise it will dump the memory.

Part1:
	Simply create a super block struct and read the info of super block. For Fat just go to where it starts and loop until 		where is ends while finding which block is empty.

Part2:
	Simply using dir_entry struct, nearly same concept as part1.

Part3: 
	Same concept of part 1 and 2, but need no how FAT mapping works (find where the block is from FAT).

Part4: 
	It is just the opposite way to do part3 but it is more complicate. We have to keep updating the FAT, directory and 		copying the content to disire directory.
