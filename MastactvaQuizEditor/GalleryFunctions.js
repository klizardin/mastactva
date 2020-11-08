.pragma library
.import QtQuick 2.0 as QtQuickModuleImportedInJS

function description_first_part(str, rmt) {
    const found = str.match(/^(.{128}[^\s]*).*/);
    console.log(found);
    if(found === null) {
        return str;
    }
    else {
        return found[1] + rmt;
    }
}

function textLFtoBr(str) {
    while(str.indexOf("\n") > -1)
    {
        str = str.replace("\n", "<br/>");
    }
    return str;
}

function clamp(v, min_, max_)
{
    return Math.min(max_, Math.max(min_, v));
}
