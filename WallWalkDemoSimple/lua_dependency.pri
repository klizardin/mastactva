isEmpty(LUA_DIR):LUA_DIR=$$(LUA_DIR)

isEmpty(LUA_DIR) {
    LUA_DIR = $$(MASTACTVA_LUA_DIR)
    isEmpty(LUA_DIR) {
       message("set LUA_DIR or MASTACTVA_LUA_DIR as environment variable or qmake variable")
    }
}

!isEmpty(LUA_DIR) {
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
