cmake_minimum_required(VERSION 3.22)
project(ticket_system)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(ticket_system file_operation.h exceptions.h B_plus_tree.h main.cpp hash_link.h account.h log.h scanner.h vector.h train.h tool.h ticket.h)
