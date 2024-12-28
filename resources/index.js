let ws;

let RQ_TIMEOUT = 10000;

/**
 * @brief Starts the websocket for client-to-client communication.
 */
function startWebSocket() {
    ws = new WebSocket("ws://" + window.location.host + "/ws");

    ws.onmessage = function(event) {
        let message = JSON.parse(event.data);
        if (message.mods) {
            message.mods.forEach(function(mod) {
                if (mod.isCheckbox) {
                    let checkbox = document.getElementById(mod.name + "-checkbox");
                    if (checkbox) {
                        checkbox.checked = mod.enabled;
                    } else {
                        let checkbox = document.createElement("input");
                        checkbox.type = "checkbox";
                        checkbox.id = mod.name + "-checkbox";
                        checkbox.checked = mod.enabled;
                        checkbox.onclick = function() { toggleMod(mod.name); };

                        let label = document.createElement("label");
                        label.className = "form-check-label";
                        label.appendChild(checkbox);
                        label.appendChild(document.createTextNode(" " + mod.name));
                        
                        let modsContainer = document.getElementById("mods-container");
                        modsContainer.appendChild(label);
                        modsContainer.appendChild(document.createElement("br"));
                    }
                } else if (mod.isButton) {
                    let button = document.getElementById(mod.name + "-button");
                    if (!button) {
                        button = document.createElement("button");
                        button.id = mod.name + "-button";
                        button.className = "btn btn-primary";
                        button.innerText = mod.name;
                        button.onclick = function() { buttonClcik(mod.name); };

                        let modsContainer = document.getElementById("mods-container");
                        modsContainer.appendChild(button);
                        modsContainer.appendChild(document.createElement("br"));
                    }
                }
            });
        }
        if (message.notification) {
            showNotification(message.notification);
        }
    };
}

/**
 * @brief Shows a notification.
 * @param {string} message - The message to show in the alert.
 */
function showNotification(message) {
    let notification = document.getElementById("notification");
    let notificationMessage = document.getElementById("notification-message");
    let notificationProgress = document.getElementById("notification-progress");

    notificationMessage.innerText = message;
    notification.style.display = "block";
    notificationProgress.style.width = "100%";

    let width = 100;
    let interval = setInterval(function() {
        width -= 2;
        notificationProgress.style.width = width + "%";
        if (width <= 0) {
            clearInterval(interval);
            notification.style.display = "none";
        }
    }, 60);
}

/**
 * @brief Sends a request to the server.
 * @param {string} method - The HTTP method.
 * @param {string} url - The endpoint on the server.
 * @param {function} onsuccess - The callback for success.
 * @param {function} onerror - The callback for errors.
 */
function send(method, url, onsuccess, onerror) {
    let xhr = new XMLHttpRequest();
    xhr.open(method, url, true);
    xhr.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
    xhr.timeout = RQ_TIMEOUT;
    xhr.onreadystatechange = function() {
        if (xhr.readyState === 4) {
            if (xhr.status === 200) {
                onsuccess(xhr.responseText);
            } else {
                onerror(xhr.status);
            }
        }
    };
    xhr.ontimeout = function() {
        onerror("Request timed out.");
    };
    xhr.onerror = function() {
        onerror("There was an error reaching the server.");
    };
    xhr.send();
}

/**
 * @brief Handles login.
 */
function login() {
    let password = document.getElementById("password").value;
    send("POST", "/login?password=" + password, function(res) {
        if (res === "success") {
            document.getElementById("login-container").style.display = "none";
            document.getElementById("panel-container").style.display = "block";
            startWebSocket();
        } else {
            showNotification("Incorrect password.");
        }
    }, function(error) {
        showNotification(error);
    });
}

/**
 * @brief Sends toggle mod request.
 * @param {string} modName - The identifier of the mod.
 */
function toggleMod(modName) {
    send("POST", "/toggle?mod=" + modName, function(res) {
        console.log(res);
    }, function(error) {
        showNotification(error);
    });
}

/**
 * @brief Sends button click request.
 * @param {string} modName - The identifier of the mod.
 */
function buttonClick(modName) {
    send("POST", "/button?mod=" + modName, function(res) {
        console.log(res);
    }, function(error) {
        showNotification(error);
    });
}