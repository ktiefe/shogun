set(STAN_STABLE_RELEASE 4b1a10bc877d941bbe0a12c198526807be27167a)

set(STAN_INCLUDE_DIR_STAN_MATH ${CMAKE_BINARY_DIR}/StanMath/src/StanMath)
set(STAN_INCLUDE_DIR_BOOST ${CMAKE_BINARY_DIR}/StanMath/src/StanMath/lib/boost_1.64.0)
set(STAN_INCLUDE_DIR_CVODES ${CMAKE_BINARY_DIR}/StanMath/src/StanMath/lib/cvodes_2.9.0/include)
set(STAN_INCLUDE_DIR_EIGEN ${CMAKE_BINARY_DIR}/StanMath/src/StanMath/lib/eigen_3.3.3)

include(ExternalProject)
ExternalProject_Add(
	StanMath
	PREFIX ${CMAKE_BINARY_DIR}/StanMath
	DOWNLOAD_DIR ${THIRD_PARTY_DIR}/StanMath
	GIT_REPOSITORY https://github.com/stan-dev/math
	GIT_TAG ${STAN_STABLE_RELEASE}
	CONFIGURE_COMMAND ""
	BUILD_COMMAND ""
	INSTALL_COMMAND ""
	LOG_DOWNLOAD ON
	)
add_dependencies(libshogun StanMath)
