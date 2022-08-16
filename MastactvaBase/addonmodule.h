/*
    Copyright 2021

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

#ifndef ADDONMODULE_H
#define ADDONMODULE_H


/*
 * it is minimal required implementation of addon module in the separate dynamic library
*/

/*
 * return addon module name
*/
extern "C" const char * addonModuleGetName();

/*
 * process input data and generate output data by addon
*/
extern "C" const char * addonModuleProcess(const char * );


#endif // ADDONMODULE_H
