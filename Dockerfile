FROM noname602/msvc_clion:vcpkg

RUN vcpkg install cpr
RUN vcpkg install cprptopp
RUN vcpkg install curl
RUN vcpkg install nlohmann-json