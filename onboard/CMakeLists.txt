cmake_minimum_required(VERSION 2.8)
project(onboard)

add_definitions(-DBOOST_ASIO_STANDALONE=1)
add_definitions(-DBOOST_ASIO_HAS_STD_ADDRESSOF=1)

include_directories(
	../3rd-party/boost/algorithm/include
	../3rd-party/boost/asio/include
	../3rd-party/boost/array/include
	../3rd-party/boost/atomic/include
	../3rd-party/boost/bind/include
	../3rd-party/boost/chrono/include
	../3rd-party/boost/config/include
	../3rd-party/boost/container/include
	../3rd-party/boost/context/include
	../3rd-party/boost/coroutine/include
	../3rd-party/boost/date_time/include
	../3rd-party/boost/detail/include
	../3rd-party/boost/exception/include
	../3rd-party/boost/functional/include
	../3rd-party/boost/function/include
	../3rd-party/boost/integer/include
	../3rd-party/boost/intrusive/include
	../3rd-party/boost/io/include
	../3rd-party/boost/iterator/include
	../3rd-party/boost/lambda/include
	../3rd-party/boost/logic/include
	../3rd-party/boost/math/include
	../3rd-party/boost/move/include
	../3rd-party/boost/mpl/include
	../3rd-party/boost/optional/include
	../3rd-party/boost/preprocessor/include
	../3rd-party/boost/range/include
	../3rd-party/boost/ratio/include
	../3rd-party/boost/regex/include
	../3rd-party/boost/serialization/include
	../3rd-party/boost/smart_ptr/include
	../3rd-party/boost/spirit/include
	../3rd-party/boost/system/include
	../3rd-party/boost/test/include
	../3rd-party/boost/thread/include
	../3rd-party/boost/tuple/include
	../3rd-party/boost/typeof/include
	../3rd-party/boost/type_traits/include
	../3rd-party/boost/units/include
	../3rd-party/boost/utility/include
)

add_executable(main src/XCI_Parrot.cpp src/AT_Command.cpp)

