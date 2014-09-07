set(i3ipc_engine_SRCS
    i3ipcengine.cpp
    i3socket.cpp
)

message(${X11_XCB_LIBRARIES})
message(${XCB_LIBRARIES})

add_library(plasma_dataengine_i3ipc MODULE ${i3ipc_engine_SRCS})
target_link_libraries(plasma_dataengine_i3ipc
    KF5::Plasma
    KF5::Service
    KF5::WindowSystem
    Qt5::Network
    Qt5::X11Extras
    ${X11_XCB_LIBRARIES}
    ${XCB_LIBRARIES}
)

kservice_desktop_to_json(plasma_dataengine_i3ipc plasma-dataengine-i3ipc.desktop)

install(TARGETS plasma_dataengine_i3ipc DESTINATION ${PLUGIN_INSTALL_DIR}/plasma/dataengine)
install(FILES plasma-dataengine-i3ipc.desktop DESTINATION ${SERVICES_INSTALL_DIR} )
