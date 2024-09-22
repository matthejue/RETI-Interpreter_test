# break *0x555555556773
# run
# display/t *instr
# display/t mode
# display/t machine_instr
# display/t load_store_mode
# display/t s
# display/t d
# display/t i

# break *0x555555556543
# run
# display/t op
# display/t opd1
# display/t opd2
# display/t opd3
# display/t machine_instr

# break main
# run

# break src/parse.c:79
# run

# break *0x555555556543
# run
# display *instr
# display/t op
# display/t opd1
# display/t opd2
# display/t opd3

# break src/interpret.c:214
# run
# display/t machine_instr
# display/t *assembly_instr

# break src/interpret_main.c:114
# run

#break src/interpret.c:234
#run
#display/t machine_instr
#display *assembly_instr

run

set pagination off
