add_executable(displayd main.c display.c drawfunc.c script.c)
target_link_libraries(displayd lua m dl)

