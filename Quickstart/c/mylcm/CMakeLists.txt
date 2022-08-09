project(libmylcm) 

# Add a library with the above sources
add_library(libmylcm mylcm_my_lcm_message_date_struct_t.c)

target_include_directories( libmylcm
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
    
)