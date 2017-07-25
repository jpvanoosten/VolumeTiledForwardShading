cmake_minimum_required(VERSION 3.9.0)

add_subdirectory( Source )

install(TARGETS FreeImage
		RUNTIME DESTINATION bin
		LIBRARY DESTINATION lib
		ARCHIVE DESTINATION lib/static
)
