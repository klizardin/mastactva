isEmpty(LUA_DIR):LUA_DIR=$$(LUA_DIR)

isEmpty(LUA_DIR) {
    isEmpty(MASTACTVA_LIBS):MASTACTVA_LIBS=$$(MASTACTVA_LIBS)
    LUA_DIR = ~/projects/lua-5.4.4
    # !isEmpty(LUA_DIR) {
    #    warning("Using LUA src dir specified at Qt Creator wizard")
    #    message("set LUA_DIR as environment variable or qmake variable to get rid of this message")
    # }
}

!isEmpty(LUA_DIR): {
    LUA_SRCDIR = $$LUA_DIR/src
} else: unix {
    exists(/usr/src/lua):LUA_SRCDIR=/usr/src/lua
    !isEmpty(LUA_SRCDIR): message("Using LUA from system")
}

requires(exists($$LUA_SRCDIR))

!isEmpty(LUA_SRCDIR) {
    INCLUDEPATH *= \
        $$LUA_SRCDIR

    LIBS += \
        -L$$LUA_SRCDIR -llua -ldl
}
