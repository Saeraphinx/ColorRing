function brightness() {
    var brightness = document.getElementById("in_bright").value;

    //validate input
    if(brightness > 255) {
        brightness = 255;
    } else if(brightness < 0) {
        brightness = 0;
    }

    location.href = 'update/brightness/' + brightness;
}