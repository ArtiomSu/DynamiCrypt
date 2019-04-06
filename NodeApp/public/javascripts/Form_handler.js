$(document).ready(function(){

    // keeps new log info scrolled to the bottom of the div
    var log_div = document.getElementById("logging");
    log_div.scrollTop = log_div.scrollHeight;

    var messages_div = document.getElementById("messages_div");
    messages_div.scrollTop = messages_div.scrollHeight;

    // makes app kinda responsive by only reloading when there is no input
    function repload_if_no_input(){
        if ($(".test_empty").filter(function() { return $(this).val(); }).length === 0) {
            location.reload();
        }
    }

    setTimeout(repload_if_no_input, 600);

    /*
    $("#show_service_form").click(function(){

        $.get("/show_service_form", function(data){
            console.log("pressed show_service_form button");
        });

    });
    */

});
