include(FetchContent)

FetchContent_Declare(
	glm
	GIT_REPOSITORY	https://github.com/g-truc/glm.git
	GIT_TAG 	1.0.3
	GIT_SHALLOW true
)

FetchContent_MakeAvailable(glm)


target_link_libraries(DDMEngine PUBLIC glm::glm)