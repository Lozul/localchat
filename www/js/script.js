// Client of localchat

// Global Variables
// ----------------------------------------------------------------------------
var messages_obj;       // Div to display all sent messages
var text_obj;           // Input for the user to enter his messages
var members_count_obj;  // Span to display current online members count
var members_obj;        // List to display current online members

var name;               // Name of the current user
var request_messages;   // XMLHttpRequest object used to request new messages
var request_members;    // XMLHttpRequest object used to request members list
var timer;              // Timer used to send 'update' request
var last_update;        // Timestamp of the previous request_messages call

var message_html;       // HTML template of a message.

// Functions
// ----------------------------------------------------------------------------

// Adds a simple String.format function.
String.prototype.format = function()
{
    f = this;
    for (k in arguments)
    {
        f = f.replace("{" + k + "}", arguments[k]);
    }
    return f;
}

// Initializes the page.
function init()
{
    // Initializes requests.
    request_messages = new XMLHttpRequest();
    request_messages.onload = request_messages_callback;
    request_messages.responseType = "text";

    request_members = new XMLHttpRequest();
    request_members.onload = request_members_callback;
    request_members.responseType = "text";

    // Gets the element to print messages.
    messages_obj = document.querySelector("div#messages");

    // Gets the element where user enter his message.
    text_obj = document.querySelector("input#text");

    // Gets the element to display curent online members count.
    members_count_obj = document.querySelector("span#membersCount");

    // Gets the element to display the list of online members.
    members_obj = document.querySelector("ul#members");

    // Gets the HTML of a message and hide the template.
    let tmp = document.querySelector("div.message");
    message_html = tmp.outerHTML;
    tmp.style.display = "none";

    // Gets the user name (set by server).
    name = document.title;
}

// Adds a new message.
function add_message(name, msg)
{
    messages_obj.innerHTML += message_html.format(name, msg);
}

// Sends a message.
function send()
{
    let msg = text_obj.value;
    text_obj.value = "";

    request_messages.open("get", "send_" + name + "_" + msg);
    request_messages.send();
}

// Callback function of the request_messages.
// The response of the request is a string of characters that represents
// the last messages receive after last_update.
function request_messages_callback()
{
    let received_msg = request_messages.responseText.split("\n");
    let len = received_msg.length;
    if (len % 2 == 0)
    {
        for (let i = 0; i < len; i += 2)
        {
            add_message(received_msg[i], received_msg[i + 1]);
        }
    }
    else
    {
        console.error("request_messages() returns an odd number.");
    }
}

// Callback function of the request_members.
// The response of the request is a string of characters that represents
// the list of currently online members.
function request_members_callback()
{
    // TODO
}
