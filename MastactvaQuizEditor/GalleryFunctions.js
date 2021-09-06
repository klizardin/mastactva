/*
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

.pragma library
.import QtQuick 2.15 as QtQuickModuleImportedInJS

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

function easingTypeFromStr(str)
{
    if("Linear" === str) { return QtQuickModuleImportedInJS.Easing.Linear; }
    else if("InQuad" === str) { return QtQuickModuleImportedInJS.Easing.InQuad; }
    else if("OutQuad" === str) { return QtQuickModuleImportedInJS.Easing.OutQuad; }
    else if("InOutQuad" === str) { return QtQuickModuleImportedInJS.Easing.InOutQuad; }
    else if("OutInQuad" === str) { return QtQuickModuleImportedInJS.Easing.OutInQuad; }
    else if("InCubic" === str) { return QtQuickModuleImportedInJS.Easing.InCubic; }
    else if("OutCubic" === str) { return QtQuickModuleImportedInJS.Easing.OutCubic; }
    else if("InOutCubic" === str) { return QtQuickModuleImportedInJS.Easing.InOutCubic; }
    else if("OutInCubic" === str) { return QtQuickModuleImportedInJS.Easing.OutInCubic; }
    else if("InQuart" === str) { return QtQuickModuleImportedInJS.Easing.InQuart; }
    else if("OutQuart" === str) { return QtQuickModuleImportedInJS.Easing.OutQuart; }
    else if("InOutQuart" === str) { return QtQuickModuleImportedInJS.Easing.InOutQuart; }
    else if("OutInQuart" === str) { return QtQuickModuleImportedInJS.Easing.OutInQuart; }
    else if("InQuint" === str) { return QtQuickModuleImportedInJS.Easing.InQuint; }
    else if("OutQuint" === str) { return QtQuickModuleImportedInJS.Easing.OutQuint; }
    else if("InOutQuint" === str) { return QtQuickModuleImportedInJS.Easing.InOutQuint; }
    else if("OutInQuint" === str) { return QtQuickModuleImportedInJS.Easing.OutInQuint; }
    else if("InSine" === str) { return QtQuickModuleImportedInJS.Easing.InSine; }
    else if("OutSine" === str) { return QtQuickModuleImportedInJS.Easing.OutSine; }
    else if("InOutSine" === str) { return QtQuickModuleImportedInJS.Easing.InOutSine; }
    else if("OutInSine" === str) { return QtQuickModuleImportedInJS.Easing.OutInSine; }
    else if("InExpo" === str) { return QtQuickModuleImportedInJS.Easing.InExpo; }
    else if("OutExpo" === str) { return QtQuickModuleImportedInJS.Easing.OutExpo; }
    else if("InOutExpo" === str) { return QtQuickModuleImportedInJS.Easing.InOutExpo; }
    else if("OutInExpo" === str) { return QtQuickModuleImportedInJS.Easing.OutInExpo; }
    else if("InCirc" === str) { return QtQuickModuleImportedInJS.Easing.InCirc; }
    else if("OutCirc" === str) { return QtQuickModuleImportedInJS.Easing.OutCirc; }
    else if("InOutCirc" === str) { return QtQuickModuleImportedInJS.Easing.InOutCirc; }
    else if("OutInCirc" === str) { return QtQuickModuleImportedInJS.Easing.OutInCirc; }
    else if("InElastic" === str) { return QtQuickModuleImportedInJS.Easing.InElastic; }
    else if("OutElastic" === str) { return QtQuickModuleImportedInJS.Easing.OutElastic; }
    else if("InOutElastic" === str) { return QtQuickModuleImportedInJS.Easing.InOutElastic; }
    else if("OutInElastic" === str) { return QtQuickModuleImportedInJS.Easing.OutInElastic; }
    else if("InBack" === str) { return QtQuickModuleImportedInJS.Easing.InBack; }
    else if("OutBack" === str) { return QtQuickModuleImportedInJS.Easing.OutBack; }
    else if("InOutBack" === str) { return QtQuickModuleImportedInJS.Easing.InOutBack; }
    else if("OutInBack" === str) { return QtQuickModuleImportedInJS.Easing.OutInBack; }
    else if("InBounce" === str) { return QtQuickModuleImportedInJS.Easing.InBounce; }
    else if("OutBounce" === str) { return QtQuickModuleImportedInJS.Easing.OutBounce; }
    else if("InOutBounce" === str) { return QtQuickModuleImportedInJS.Easing.InOutBounce; }
    else if("OutInBounce" === str) { return QtQuickModuleImportedInJS.Easing.OutInBounce; }
    else { return QtQuickModuleImportedInJS.Easing.Linear; }
}
