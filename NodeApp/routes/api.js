var express  = require('express');
var request = require('request');
var router = express.Router();
var url = require('url');

// API potential error messages for encryption
const DYNAMICRYPT_API_ERROR_WAIT = "wait";
const DYNAMICRYPT_API_ERROR_SERVICE_404 = "service_not_found";
const DYNAMICRYPT_API_ERROR_DECRYPTION = "failed_decrypt";

var show_service_form = true;
var show_partner_form = false;
var show_message_box = false;
var current_service_name = null;
var current_partner_name = null;
var can_start_sync = false;
var sync_is_running = false;
var show_stop_sync = false;
var logs_at_bottom_of_screen = "";
var services = {};

//will store objects like this {encrypted: "FHFHRIUFHIYRIGHDFHFHIDFUG", decrypted: "hello there ", received: 1}
// received 1 means that partner nodejs App sent it over and this nodejs app decrypted it
// received 0 means the message typed in the box was encrypted and is now shown in the table
// this is just so the colour of sent and received messages is different
var messages_info = [];

/*
messages_info.push({encrypted: "FHFHRIUFHIVHEIVHUEFGHUFHERIUFH483975YFHDFHJVBFE487TYRIGHDFHFHIDFUG", decrypted: "hello there sunny jim", received: 1});
messages_info.push({encrypted: "FHFHRIUFHIVHEIVHUEFfghdfhdsfhHFJDHFKJFSDHJKVBFE487TYRIGHDFHFHIDFUG", decrypted: "sunny jim", received: 1});
messages_info.push({encrypted: "FHFHRIUFHIVHEIVHDGRHTHRHRTHRTHRHRKLLPYHFHFGHFGHFDGDFGFVVVEFR456ISS", decrypted: "TEXT TEXT TEXT", received: 1});
messages_info.push({encrypted: "GDGDFG456567GRTGRTGTRT4YT45RTGRT546TGTRGRT65756RTHTRY56RTHTFGHSIISS", decrypted: "Typed this out", received: 0});
messages_info.push({encrypted: "FHFHRIUFHIVHEIVDHGDFJKFH38945Y34897YIUGHE34IU7TY34897HERIU457IU5457", decrypted: "loops are nice", received: 0});
messages_info.push({encrypted: "FHFHRIUFHGHBEJGHRE783487RTHYUIEFHERY4387TRY3498FEDFIUHJGNDFKJJSDFJNSLKJ4JGH5789TY45398TYREGHTH845TH4895T8459T84359034TUY9345ELTU34Y09P;FPER9TU3409348FEDRFJ94358TYER87GHERFUIH4587TYE98FYH495", decrypted: "long line very long indeed long line very long indeed long line very long indeed long line very long indeed long line very long indeed long line very long indeed long line very long indeed long line very long indeed long line very long indeed long line very long indeed", received: 1});
messages_info.push({encrypted: "45Y34897YIUGHE34IU7TY34897HE", decrypted: "e", received: 0});
messages_info.push({encrypted: "FHFHRIUFHIVHEIVHUEFGHUFHERIUFH483975YFHDFHJVBFE487TYRIGHDFHFHIDFUG", decrypted: "hello there sunny jim", received: 1});
messages_info.push({encrypted: "FHFHRIUFHIVHEIVHUEFfghdfhdsfhHFJDHFKJFSDHJKVBFE487TYRIGHDFHFHIDFUG", decrypted: "sunny jim", received: 2});
messages_info.push({encrypted: "FHFHRIUFHIVHEIVHDGRHTHRHRTHRTHRHRKLLPYHFHFGHFGHFDGDFGFVVVEFR456ISS", decrypted: "TEXT TEXT TEXT", received: 1});
messages_info.push({encrypted: "GDGDFG456567GRTGRTGTRT4YT45RTGRT546TGTRGRT65756RTHTRY56RTHTFGHSIISS", decrypted: "Typed this out", received: 0});
messages_info.push({encrypted: "FHFHRIUFHIVHEIVDHGDFJKFH38945Y34897YIUGHE34IU7TY34897HERIU457IU5457", decrypted: "loops are nice", received: 0});
messages_info.push({encrypted: "FHFHRIUFHGHBEJGHRE783487RTHYUIEFHERY4387TRY3498FEDFIUHJGNDFKJJSDFJNSLKJ4JGH5789TY45398TYREGHTH845TH4895T8459T84359034TUY9345ELTU34Y09P;FPER9TU3409348FEDRFJ94358TYER87GHERFUIH4587TYE98FYH495", decrypted: "long line very long indeed long line very long indeed long line very long indeed long line very long indeed long line very long indeed long line very long indeed long line very long indeed long line very long indeed long line very long indeed long line very long indeed", received: 1});
messages_info.push({encrypted: "45Y34897YIUGHE34IU7TY34897HE", decrypted: "e", received: 0});
*/
router.get('/', function(req, res, next) {
    res.render('index', { current_service_name: current_service_name,
        show_service_form: show_service_form,
        show_partner_form: show_partner_form,
        logs_at_bottom_of_screen: logs_at_bottom_of_screen,
        current_partner_name: current_partner_name,
        can_start_sync: can_start_sync,
        messages_info: messages_info,
        show_message_box: show_message_box,
        sync_is_running: sync_is_running,
        show_stop_sync: show_stop_sync,
    });
});

router.get('/reset_all_variables', function(req, res, next) {
    show_service_form = true;
    show_partner_form = false;
    current_service_name = null;
    current_partner_name = null;
    can_start_sync = false;
    services = {};
    messages_info = [];
    logs_at_bottom_of_screen += "<p class='log_ok'>/reset_all_variables variables are now reset</p>";
    show_message_box = false;
    sync_is_running = false;
    show_stop_sync = false;
    return res.redirect('/');
});

router.get('/show_service_form', function(req, res, next) {
    show_service_form = true;
    show_partner_form = false;
    console.log("button clicked ", show_service_form);
    return res.redirect('/');
});

//get data from init api route.
router.post('/form_get_service_name', function (req,res,next) {
    var temp = req.headers.host.split(":");
    console.log("req host = ", temp[0], " port = ", temp[1]);
    //var q = url.parse(req.headers.host, true);
    //console.log(q);


    var data = req.body;
    var service_name = data.Service_name;
    current_service_name = service_name;
    var api_port = data.API_port;
    var api_address = data.API_Address;

    var this_node_app_address = temp[0];
    var this_node_app_port = temp[1];
    //console.log(data);

    show_service_form = false;
    request({
        url: "http://"+api_address+":"+api_port+"/v1/options/init",
        method: 'POST',
        json: {service_name: service_name}
    }, function (error, response, body) {
        console.log("error", error);
        console.log("body", body);
        if(error){
            logs_at_bottom_of_screen += "<p class='log_error'>failed to connect to API /v1/options/init</p>";
            show_service_form = true;
        }else{
            var new_service_name = body.service_name;
            current_service_name = new_service_name;
            var address_of_this_tpm = body.address_of_this_tpm;
            var port_of_this_tpm = body.port_of_this_tpm;
            show_partner_form = true;
            services[new_service_name] = {service_name:new_service_name,
                address_of_this_tpm:address_of_this_tpm,
                port_of_this_tpm:port_of_this_tpm,
                api_port: api_port,
                api_address: api_address,
                address_of_partner_tpm: null,
                port_of_partner_tpm: null,
                partner_name: null,
                partner_address: null,
                partner_port: null,
                this_node_app_address: this_node_app_address,
                this_node_app_port: this_node_app_port
            };
        }
        //console.log(services[current_service_name]);
        return res.redirect('/');
    });
});

router.post('/form_send_to_partner', function (req,res,next) {
    // other app already submitted request so no need to do anything here
    if(show_partner_form === false){
        logs_at_bottom_of_screen += "<p class='log_warn_not_severe'>/form_send_to_partner other node APP already submitted this request and details are shared</p>";
        return res.redirect('/');
    }

    var data = req.body;

    var partner_port = data.port;
    var partner_address = data.address;
    //console.log(data);

    //find empty service
    var aproporiate_service;
    Object.keys(services).forEach(function(key) {
        if(services[key].partner_name == null){
            aproporiate_service = key;
        }
    });
    current_service_name = aproporiate_service;
    services[current_service_name].partner_address = partner_address;
    services[current_service_name].partner_port = partner_port;

    request({
        url: "http://"+partner_address+":"+partner_port+"/get_details",
        method: 'POST',
        json: {partner_address: services[current_service_name].this_node_app_address, partner_port: services[current_service_name].this_node_app_port, service_name: services[aproporiate_service].service_name, address_of_this_tpm: services[aproporiate_service].address_of_this_tpm, port_of_this_tpm: services[aproporiate_service].port_of_this_tpm}
    }, function (error, response, body) {
        if(error){
            logs_at_bottom_of_screen += "<p class='log_error'>failed to connect to other node APP at /get_details </p>";
            show_partner_form = true;
            return res.redirect('/');
        }else{

            services[aproporiate_service].address_of_partner_tpm = body.address_of_this_tpm;
            services[aproporiate_service].port_of_partner_tpm = body.port_of_this_tpm;
            services[aproporiate_service].partner_name =  body.service_name;

            current_partner_name = services[aproporiate_service].partner_name;

            show_partner_form = false;

            // now need to update partner in the API
            request({
                url: "http://"+services[aproporiate_service].api_address+":"+services[aproporiate_service].api_port+"/v1/options/init_config",
                method: 'POST',
                json: {service_name: services[aproporiate_service].service_name, partner_name: services[aproporiate_service].partner_name}
            }, function (error, response, body) {
                console.log("error", error);
                console.log("body", body);
                if(error){
                    logs_at_bottom_of_screen += "<p class='log_error'>failed to connect to API /v1/options/init_config</p>";
                    return res.redirect('/');
                }else{
                    if(body.status === 0){
                        // failed to update
                        logs_at_bottom_of_screen += "<p class='log_error'>failed to update partner name API /v1/options/init_config</p>";
                        return res.redirect('/');
                    }else{
                        can_start_sync = true;
                        show_message_box = true;
                        return res.redirect('/');
                    }
                }
            });
        }
    });
});

router.post('/get_details', function (req,res,next) {
    var data = req.body;

    var partner_port_tpm = data.port_of_this_tpm;
    var partner_address_tom = data.address_of_this_tpm;
    var partner_name = data.service_name;
    var partner_port = data.partner_port;
    var partner_address = data.partner_address;
    //console.log(data);
    //find empty service
    var aproporiate_service;
    Object.keys(services).forEach(function(key) {
        if(services[key].partner_name == null){
            aproporiate_service = key;
        }
    });
    services[aproporiate_service].address_of_partner_tpm = partner_address_tom;
    services[aproporiate_service].port_of_partner_tpm = partner_port_tpm;
    services[aproporiate_service].partner_name =  partner_name;
    services[aproporiate_service].partner_port = partner_port;
    services[aproporiate_service].partner_address =  partner_address;
    current_partner_name = services[aproporiate_service].partner_name;
    current_service_name = aproporiate_service;
    show_partner_form = false;

    // need to update the partner name in the Api.
    request({
        url: "http://"+services[aproporiate_service].api_address+":"+services[aproporiate_service].api_port+"/v1/options/init_config",
        method: 'POST',
        json: {service_name: services[aproporiate_service].service_name, partner_name: services[aproporiate_service].partner_name}
    }, function (error, response, body) {
        console.log("error", error);
        console.log("body", body);
        if(error){
            logs_at_bottom_of_screen += "<p class='log_error'>failed to connect to API /v1/options/init_config</p>";
            res.json({service_name: services[aproporiate_service].service_name, address_of_this_tpm: services[aproporiate_service].address_of_this_tpm, port_of_this_tpm: services[aproporiate_service].port_of_this_tpm});
        }else{
            if(body.status === 0){
                // failed to update
                logs_at_bottom_of_screen += "<p class='log_error'>failed to update partner name API /v1/options/init_config</p>";

            }else{

            }
            show_message_box = true;
            res.json({service_name: services[aproporiate_service].service_name, address_of_this_tpm: services[aproporiate_service].address_of_this_tpm, port_of_this_tpm: services[aproporiate_service].port_of_this_tpm});

        }
    });

});

router.get('/start_sync', function(req, res, next) {
    if(can_start_sync === false){
        return res.redirect('/');
    }
    can_start_sync = false;
    request({
        url: "http://"+services[current_service_name].api_address+":"+services[current_service_name].api_port+"/v1/options/sync",
        method: 'POST',
        json: {service_name: services[current_service_name].service_name, partner_name: services[current_service_name].partner_name, address_of_other_tpm: services[current_service_name].address_of_partner_tpm, port_of_other_tpm: services[current_service_name].port_of_partner_tpm }
    }, function (error, response, body) {
        console.log("error", error);
        console.log("body", body);
        if(error){
            logs_at_bottom_of_screen += "<p class='log_error'>failed to connect to API /v1/options/sync</p>";
            can_start_sync = true;
            return res.redirect('/');
        }else{
            if(body.status === 0){
                // failed to update
                can_start_sync = true;
                logs_at_bottom_of_screen += "<p class='log_error'>failed to start sync API /v1/options/sync</p>";

            }else{
                sync_is_running = true;
                show_stop_sync = true;
            }
            return res.redirect('/');
        }
    });
});

//will be called from form encrypt messsage and send to partners /decrypt route
router.post('/encrypt', function (req,res,next) {
    //console.log("current service ", current_service_name);
    //console.log(services);
    if(show_message_box === false){
        return res.redirect('/');
    }
    var data = req.body;

    var message = data.message;
    var mode = data.encrypt_mode;

    console.log("going to encrypt message:",message,"\nusing mode:",mode);

    request({
        url: "http://"+services[current_service_name].api_address+":"+services[current_service_name].api_port+"/v1/options/encrypt",
        method: 'POST',
        json: {service_name: services[current_service_name].service_name, message: message, operation: 0, mode: parseInt(mode, 10)}
    }, function (error, response, body) {
        console.log("error", error);
        console.log("body", body);
        if(error){
            logs_at_bottom_of_screen += "<p class='log_error'>failed to connect to API /v1/options/encrypt</p>";
            return res.redirect('/');
        }else{
            if(body.error === DYNAMICRYPT_API_ERROR_WAIT){
                logs_at_bottom_of_screen += "<p class='log_warn_not_severe'>no suitable keys found going to waiting</p>";
                return res.redirect('/');
            }else if(body.error === DYNAMICRYPT_API_ERROR_SERVICE_404){
                logs_at_bottom_of_screen += "<p class='log_error'>API /v1/options/encrypt service name not found</p>";
                return res.redirect('/');
            }else if(body.error === DYNAMICRYPT_API_ERROR_DECRYPTION){
                logs_at_bottom_of_screen += "<p class='log_error'>API /v1/options/encrypt failed to decrypt</p>";
                return res.redirect('/');
            }else{
                //encrypted ok
                var encrypted_text = body.message;
                var hash = body.hash;
                mode = body.mode;

                console.log("encrypted message ok: ", encrypted_text, "\nhash:", hash, "\nmode:", mode);

                //send to other node App to decrypt
                request({
                    url: "http://"+services[current_service_name].partner_address+":"+services[current_service_name].partner_port+"/decrypt",
                    method: 'POST',
                    json: {message: encrypted_text, hash: hash, mode: mode}
                }, function (error, response, body) {
                    if(error){
                        logs_at_bottom_of_screen += "<p class='log_error'>failed to connect to other node APP at /decrypt </p>";
                        return res.redirect('/');
                    }else{
                        if(body.error === "not ready"){
                            logs_at_bottom_of_screen += "<p class='log_warn_not_severe'>other node APP at /decrypt is not ready to take decryption requests </p>";
                            return res.redirect('/');
                        }else if(body.error === "failed_to_connect"){
                            logs_at_bottom_of_screen += "<p class='log_error'>other node App failed to connect to API /v1/options/encrypt</p>";
                            return res.redirect('/');
                        }
                        else if(body.error === "no_keys_found"){
                            logs_at_bottom_of_screen += "<p class='log_warn_not_severe'>other node APP no suitable keys found for decryption going to waiting (shouldn't happen)</p>";
                            return res.redirect('/');
                        }
                        else if(body.error === "service_name_not_found"){
                            logs_at_bottom_of_screen += "<p class='log_error'>other node App /v1/options/encrypt service name not found</p>";
                            return res.redirect('/');
                        }
                        else if(body.error === "failed_decrypt"){
                            logs_at_bottom_of_screen += "<p class='log_error'>other node App /v1/options/encrypt failed to decrypt</p>";
                            messages_info.push({encrypted: encrypted_text, decrypted: message, received: 2});
                            return res.redirect('/');
                        }

                        //decrypted ok
                        if(body.status === "ok"){
                            messages_info.push({encrypted: encrypted_text, decrypted: message, received: 0});
                        }else{
                            logs_at_bottom_of_screen += "<p class='log_error'>API /v1/options/encrypt failed to decrypt unknown error"+body.why+"</p>";
                        }

                        return res.redirect('/');
                    }
                });


            }


        }

    });



});

//decrypt message sent from partner
router.post('/decrypt', function (req,res,next) {
    if(show_message_box === false){
        res.json({error: "not ready"});
    }

    var data = req.body;

    var message = data.message;
    var mode = data.mode;
    var hash = data.hash;

    console.log("going to decrypt message:", message,"\nusing hash:", hash, "\nusing mode:", mode);

    request({
        url: "http://"+services[current_service_name].api_address+":"+services[current_service_name].api_port+"/v1/options/encrypt",
        method: 'POST',
        json: {service_name: services[current_service_name].service_name, message: message, operation: 1, mode: parseInt(mode, 10), hash: hash}
    }, function (error, response, body) {
        console.log("error", error);
        console.log("body", body);
        if(error){
            logs_at_bottom_of_screen += "<p class='log_error'>failed to connect to API /v1/options/encrypt</p>";
            res.json({error: "failed_to_connect"});
        }else{
            if(body.error === DYNAMICRYPT_API_ERROR_WAIT){
                logs_at_bottom_of_screen += "<p class='log_warn_not_severe'>no suitable keys found for decryption going to waiting (shouldn't happen)</p>";
                res.json({error: "no_keys_found"});
            }else if(body.error === DYNAMICRYPT_API_ERROR_SERVICE_404){
                logs_at_bottom_of_screen += "<p class='log_error'>API /v1/options/encrypt service name not found</p>";
                res.json({error: "service_name_not_found"});
            }else if(body.error === DYNAMICRYPT_API_ERROR_DECRYPTION){
                logs_at_bottom_of_screen += "<p class='log_error'>API /v1/options/encrypt failed to decrypt</p>";
                messages_info.push({encrypted: message, decrypted: "failed to decrypt", received: 2});
                res.json({error: "failed_decrypt"});
            }else{
                //encrypted ok
                var plaintext = body.message;
                if(body.message){
                    // second nodejs app that didnt start the sync will only know if sync is running if it decrypts something successfully.
                    sync_is_running = true;
                    show_stop_sync = true;
                    console.log("decrypted message ok: ", plaintext);
                    messages_info.push({encrypted: message, decrypted: plaintext, received: 1});

                    res.json({status: "ok"});
                }else{
                    console.log("unknown error when decrypting");
                    logs_at_bottom_of_screen += "<p class='log_error'>API /v1/options/encrypt failed to decrypt unknown error"+body+"</p>";
                    res.json({status: "not_good", why: body});
                }

            }

        }

    });

});

router.get('/stop_sync', function(req, res, next) {
    if(sync_is_running === false){
        return res.redirect('/');
    }
    request({
        url: "http://"+services[current_service_name].api_address+":"+services[current_service_name].api_port+"/v1/options/exit",
        method: 'POST',
        json: {service_name: services[current_service_name].service_name}
    }, function (error, response, body) {
        console.log("error", error);
        console.log("body", body);
        if(error){
            logs_at_bottom_of_screen += "<p class='log_error'>failed to connect to API /v1/options/exit</p>";
            return res.redirect('/');
        }else{
            if(body.status === "ok"){

                //tell other node app sync stopped
                request({
                    url: "http://"+services[current_service_name].partner_address+":"+services[current_service_name].partner_port+"/stop_sync_inform_other_node_app",
                    method: 'GET'
                }, function (error, response, body) {

                });

                logs_at_bottom_of_screen += "<p class='log_ok'>successfully stopped syncing</p>";
                show_service_form = true;
                show_partner_form = false;
                current_service_name = null;
                current_partner_name = null;
                can_start_sync = false;
                services = {};
                show_message_box = false;
                sync_is_running = false;
                show_stop_sync = false;


            }else{
                logs_at_bottom_of_screen += "<p class='log_error'> API /v1/options/exit "+ body.error +"</p>";
            }
            return res.redirect('/');
        }
    });
});

// need to tell other node app manually that u stopped the sync otherwise if they try to stop it they will get error saying service_name not found since that is removed after stopping the sync
// here completely trust the other app to not lie fine for demo purposes change for real application
router.get('/stop_sync_inform_other_node_app', function(req, res, next) {
    if(sync_is_running === false){
        res.json({ok: 0});
    }
    logs_at_bottom_of_screen += "<p class='log_ok'>successfully stopped syncing</p>";
    show_service_form = true;
    show_partner_form = false;
    current_service_name = null;
    current_partner_name = null;
    can_start_sync = false;
    services = {};
    show_message_box = false;
    sync_is_running = false;
    show_stop_sync = false;

    res.json({ok: 1});

});

module.exports = router;