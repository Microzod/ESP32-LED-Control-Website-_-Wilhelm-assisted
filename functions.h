#ifndef _FUNCTIONS_H_
#define _FUNCTIONS_H_

/*
Copyright (C) 2021 wk & david
This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, version 3.
The above copyright notice, this permission notice and the word "NIGGER" shall be included in all copies or substantial portions of the Software.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of 
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>
*/

String formatBytes(size_t bytes) {
    if(bytes < 1024) {
        return String(bytes) + "B";
    } else if(bytes < (1024 * 1024)) {
        return String(bytes / 1024.0) + "KB";
    } else if(bytes < (1024 * 1024 * 1024)) {
        return String(bytes / 1024.0 / 1024.0) + "MB";
    } else {
        return String(bytes / 1024.0 / 1024.0 / 1024.0) + "GB";
    }
}

String getContentType(String filename) {
    if(filename.endsWith(".htm")) {
        return "text/html";
    } else if(filename.endsWith(".html")) {
        return "text/html";
    } else if(filename.endsWith(".css")) {
        return "text/css";
    } else if(filename.endsWith(".js")) {
        return "application/javascript";
    } else if(filename.endsWith(".png")) {
        return "image/png";
    } else if(filename.endsWith(".gif")) {
        return "image/gif";
    } else if(filename.endsWith(".jpg")) {
        return "image/jpeg";
    } else if(filename.endsWith(".ico")) {
        return "image/x-icon";
    } else if(filename.endsWith(".xml")) {
        return "text/xml";
    } else if(filename.endsWith(".pdf")) {
        return "application/x-pdf";
    } else if(filename.endsWith(".zip")) {
        return "application/x-zip";
    } else if(filename.endsWith(".gz")) {
        return "application/x-gzip";
    }
    return "text/plain";
}

const char* indexData PROGMEM = R"=====(
<!DOCTYPE html>
<html lang="en">
    <head>

        <title>
            ESP LED
        </title>

        <link rel="stylesheet" href="https://maxcdn.bootstrapcdn.com/bootstrap/3.4.1/css/bootstrap.min.css">

        <style>
            body {
                background-color: #212121;
                height: 100%;
                color: #c1c1c1; 
            }

            h1,h2,h3 {
                color: #c1c1c1;
            }

            nav {
                width: 100%;
                -webkit-box-shadow: 0px 5px 13px -1px rgba(0,0,0,0.46);
                box-shadow: 0px 5px 13px -1px rgba(0,0,0,0.46);
            }

            .ql-hidden {
                display: none;
            }

            .navbar-header {
                position: relative;
                border: none;
                margin-bottom: 0px;
            }

            #header {
                position: relative;
                
            }

            .navbar {
                border: none;
                margin-bottom: 0;
                border-radius: 0;
            }

            .row.content {
                height: 450px;
            }

            .sidenav {
                padding-top: 20px;
                background-color: #f1f1f1;
                height: 100%;
            }

            .logo {
                width: 100px;
                position: relative;
                top: -10px;
            }

            footer {
                background-color: none;
                color: white;
                padding: 15px;
                width: 80%;
            }

            @media screen and (max-width: 767px) {
                .sidenav {
                    height: auto;
                    padding: 15px;
                }
            .row.content {height:auto;}
            }

            .midContent {
                background-color: #383838;
            }

            .midWrapper {
                height: 100%;
                margin-top: 20px;
            }

            .midContentWrapper {
                background-color: #272727;
                width: 86%;
                -webkit-box-shadow: 0px 5px 13px -1px rgba(0,0,0,0.46);
            box-shadow: 0px 5px 13px -1px rgba(0,0,0,0.46);
            }

            #editMsg {
                position: absolute;
                width: 200px;
                height: 40px;
                border: 2px solid #212121;
                background-color: #272727;
                color: #c1c1c1;
            }
    
            #topControl {
                position: realtive; 
                border: 0px solid black;
                padding: 10px 0px 10px 5px;
            }
          
        h1 { 
            font-family: Helvetica Neue, Helvetica, Arial, sans-serif; font-size: 24px; font-style: normal; font-variant: normal; font-weight: 700; line-height: 26.4px; } h3 { font-family: Helvetica Neue, Helvetica, Arial, sans-serif; font-size: 14px; font-style: normal; font-variant: normal; font-weight: 700; line-height: 15.4px; } p { font-family: Helvetica Neue, Helvetica, Arial, sans-serif; font-size: 14px; font-style: normal; font-variant: normal; font-weight: 400; line-height: 20px; } blockquote { font-family: Helvetica Neue, Helvetica, Arial, sans-serif; font-size: 21px; font-style: normal; font-variant: normal; font-weight: 400; line-height: 30px; } pre { font-family: Helvetica Neue, Helvetica, Arial, sans-serif; font-size: 13px; font-style: normal; font-variant: normal; font-weight: 400; line-height: 18.5667px; }\ 
        
        }
        </style>



        <meta charset="utf-8">
        <meta name="viewport" content="width=device-width, initial-scale=1">
        <script src="https://ajax.googleapis.com/ajax/libs/jquery/3.5.1/jquery.min.js"></script>
        <script src="https://maxcdn.bootstrapcdn.com/bootstrap/3.4.1/js/bootstrap.min.js"></script>

        <script>
            //alert("nigger");
            var url = "http://esp.lan/config.json";
            //var url = \"http://esp.lan/config.json\";\
            $.getJSON(url, function(data) {
                for (var i in data) {
                    //alert(i);
                    $('input[name="'+i+'"]').val(data[i]);
                }
            });

            function incVal(incItem, step) {
                obj = $('#' + incItem);
                x = parseInt(obj.val());
                y = x+step;            
                obj.val(y);
                
            }

            function decrVal(decrItem, step) {
                obj = $('#' + decrItem);
                x = parseInt(obj.val());
                y = x-step;            
                obj.val(y);

                function incIntensityVal(incItem, stepItem) {
                obj = $('#' + incItem);
                x = parseInt(obj.val());
                objStep = $('#' + stepItem);
                step = parseInt(objStep.val());
                y = x+step;            
                obj.val(y);
                
            }

            function decrIntensityVal(decrItem, stepItem) {
                obj = $('#' + decrItem);
                x = parseInt(obj.val());
                objStep = $('#' + stepItem);
                step = parseInt(objStep.val());
                y = x-step;            
                obj.val(y);
            }
        </script>

    </head>

    <body>
        <nav class="navbar navbar-inverse">
                <div class="navbar-header">
                    <h3> &nbsp;&nbsp;ESP LED</h3>
                </div>
        </nav>

        <div class="container-fluid text-center midWrapper">    
            <div class="container-fluid midContentWrapper">
                <div class="row content">
                    <div class="col-sm-8 text-left"> 
                        <div id="topControl">
                         <!--   <h4>Status</h4>
                            <a href="?status=on"><button>ON</button></a>
                            <a href="?status=off"><button>OFF</button></a>    
                            <a href="?status=auto"><button>AUTO</button></a>--> 
                        </div>
                        <form method="GET" action="">    
                            <div id="bottomControl">
                                <table>
                                    <tr>
                                        <td>
                                            <label for="status">Status</label>
                                        </td>
                                        <td>
                                            <select name="status" id="status">
                                                <option value="on">On</option>
                                                <option value="auto">Auto</option>
                                                <option value="off">Off</option>
                                            </select>
                                        </td>
                                    </tr>
                                    <tr>
                                        <td>
                                            <label for="startOfDay">Start of Day</label>
                                        </td>
                                        <td>
                                            <input type="text" name="startOfDay" id="startOfDay" value="08:00" />
                                        </td>
                                    </tr>
                                    <tr>
                                        <td>
                                            <label for="endOfDay">End of Day</label>
                                        </td>
                                        <td>
                                            <input type="text" name="endOfDay" id="endOfDay" value="00:00" />
                                        </td>
                                    </tr>
                                    <tr>
                                        <td>
                                            <label for="rampTime">Ramp-Up Time</label>

                                        </td>
                                        <td>
                                            <input type="text" name="rampTime" id="rampTime" value="30" />
                                        </td>
                                    </tr>
                                    <tr>
                                        <td>
                                            <label for="rampStopIntensity">Ramp-Up Stop Intensity(%)</label>

                                        </td>
                                        <td>
                                            <input type="text" name="rampStopIntensity" id="rampStopIntensity" value="50.0" />
                                        </td>
                                    </tr>
                                    <tr>
                                        <td>
                                            <label for="cct">CCT(Kelvin)</label>

                                        </td>
                                        <td>
                                            <p class="btn btn-primary btn-sm" onclick="decrVal('cct', 100)">-</p>
                                            <input type="text" name="cct" id="cct" value="3800" />
                                            <p class="btn btn-primary btn-sm" onclick="incVal('cct',100)">+</p>

                                        </td>
                                    </tr>
                                    <tr>
                                        <td>
                                            <label for="intensityStep">Intensity Step Size</label>

                                        </td>
                                        <td>
                                            <input type="text" name="intensityStep" id="intensityStep" value="10" />
                                        </td>
                                    </tr>
                                    <tr>
                                        <td>
                                            <label for="intensityControl">Intensity Control</label>

                                        </td>
                                        <td>
                                            <p class="btn btn-primary btn-sm" onclick="decrIntensityVal('intensityControl', 'intensityStep')">-</p>
                                            <input type="text" name="intensityControl" id="intensityControl" value="0" />
                                            <p class="btn btn-primary btn-sm" onclick="incIntensityVal('intensityControl', 'intensityStep')">+</p>
                                        </td>
                                    </tr>
                                    <tr>
                                    <tr>
                                        <td>
                                            <input type="submit" name="send" id="send" value="SAVE" />
                                        </td>
                                    </tr>

                                </table>
                            </div>    
                        </form>
                    </div>
                </div>
            </div>
        </div>

        <footer class="container-fluid text-left fixed-bottom">
            <p>&copy; Wille+</p>
        </footer>
    </body>
</html>

)=====";



#endif // _FUNCTIONS_H_