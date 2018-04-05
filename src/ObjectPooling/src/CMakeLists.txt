
file(GLOB PROJ_HEADERS
    "*.h"
    "*.hpp"
)

file(GLOB PROJ_SRC
    "*.cpp"
)

set(APP_HEADERS
)

set(APP_SOURCES
  app/main.cpp
)

set(APPLICATION_NAME ${PROJECT_NAME}App)
add_executable(${APPLICATION_NAME} ${APP_HEADERS} ${APP_SOURCES} ${PROJ_HEADERS} ${PROJ_SRC})
