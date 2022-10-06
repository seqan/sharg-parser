/* Jongkyu Kim (j.kim@fu-berlin.de), 2016.01.12
   Adaptations by Enrico Seiler (enrico.seiler@fu-berlin.de), 2020 */

function changeVersion(form_id)
{
    // Get the base url without version information, e.g. "https://docs.seqan.de/seqan"
    var current_script_url = document.scripts[document.scripts.length - 1].src;
    var base_url = current_script_url.split('/').slice(0, -2).join('/');

    // Get the current page, e.g. "index.html"
    var full_url = window.top.location.href;
    var current_page = full_url.substring(full_url.indexOf("/",8) + 1);

    // Get the selected version
    var form = document.getElementById(form_id);
    var version = form.options[form.selectedIndex].value;

    // Check if the current page is valid with the selected version
    var proposed_url = "https://sharg" + version + ".vercel.app/" + current_page;
    var request = new XMLHttpRequest();
    request.open('GET', proposed_url, false);
    request.send();
    // If the URL is invalid, redirect to main page of the selected version
    // If htaccess is configured to redirect invalid URLs to the base domain,
    // no 404 is returned, hence the second condition
    if (request.status === 404 || request.responseURL == window.location.origin + '/')
    {
        proposed_url = base_url + '/' + version;
    }

    // Load the proper page
    window.top.location.href = proposed_url;
}

function addVersionSelection()
{
    // add HTMLs
    var version_select = document.createElement("select");
    var version_div = document.createElement("div");

    version_select.setAttribute("id","version_select");
    version_div.setAttribute("style","vertical-align:middle; text-align:right;");
    version_div.appendChild(document.createTextNode("Version: "));
    version_div.appendChild(version_select);
    document.getElementById("list_bottom_right").appendChild(version_div);

    version_select.addEventListener("change", function(){changeVersion(this.id);}, false);

    // current selection is..
    cur_sel = window.location.pathname.split("/")[2];

    var op = document.createElement("option");
    op.value = "";
    op.text = "main";
    op.selected = ( "main" == cur_sel ) ? true : false;
    version_select.add(op);

    var op = document.createElement("option");
    op.value = "-100";
    op.text = "1.0.0";
    op.selected = ( "1.0.0" == cur_sel ) ? true : false;
    version_select.add(op);
}

addVersionSelection();
