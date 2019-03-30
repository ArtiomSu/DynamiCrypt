var express  = require('express');
var request = require('request');
var router = express.Router();

var show_service_form = true;
var show_partner_form = false;
var current_service_name = null;
var current_partner_name = null;
var can_start_sync = false;
var logs_at_bottom_of_screen = "";
var services = {};

router.get('/', function(req, res, next) {
    res.render('index', { current_service_name: current_service_name,
        show_service_form: show_service_form,
        show_partner_form: show_partner_form,
        logs_at_bottom_of_screen: logs_at_bottom_of_screen,
        current_partner_name: current_partner_name,
        can_start_sync: can_start_sync,
    });
});

router.get('/reset_all_variables', function(req, res, next) {
    show_service_form = true;
    show_partner_form = false;
    current_service_name = null;
    current_partner_name = null;
    can_start_sync = false;
    services = {};
    logs_at_bottom_of_screen += "<p class='log_ok'>/reset_all_variables variables are now reset</p>";
    res.redirect('/');
});

router.get('/show_service_form', function(req, res, next) {
    show_service_form = true;
    show_partner_form = false;
    console.log("button clicked ", show_service_form);
    res.redirect('/');
});

//get data from init api route.
router.post('/form_get_service_name', function (req,res,next) {
    var data = req.body;
    var service_name = data.Service_name;
    var current_service_name = service_name;
    var api_port = data.API_port;
    var api_address = data.API_Address;
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
            services[new_service_name] = {service_name:service_name,
                address_of_this_tpm:address_of_this_tpm,
                port_of_this_tpm:port_of_this_tpm,
                api_port: api_port,
                api_address: api_address,
                address_of_partner_tpm: null,
                port_of_partner_tpm: null,
                partner_name: null
            };
        }
        res.redirect('/');
    });
});

router.post('/form_send_to_partner', function (req,res,next) {
    // other app already submitted request so no need to do anything here
    if(show_partner_form === false){
        logs_at_bottom_of_screen += "<p class='log_warn_not_severe'>/form_send_to_partner other node APP already submitted this request and details are shared</p>";
        res.redirect('/');
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

    request({
        url: "http://"+partner_address+":"+partner_port+"/get_details",
        method: 'POST',
        json: {service_name: services[aproporiate_service].service_name, address_of_this_tpm: services[aproporiate_service].address_of_this_tpm, port_of_this_tpm: services[aproporiate_service].port_of_this_tpm}
    }, function (error, response, body) {
        if(error){
            logs_at_bottom_of_screen += "<p class='log_error'>failed to connect to other node APP at /get_details </p>";
            show_partner_form = true;
            res.redirect('/');
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
                    res.redirect('/');
                }else{
                    if(body.status === 0){
                        // failed to update
                        logs_at_bottom_of_screen += "<p class='log_error'>failed to update partner name API /v1/options/init_config</p>";
                        res.redirect('/');
                    }else{
                        can_start_sync = true;
                        res.redirect('/');
                    }
                }
            });
        }
    });
});

router.post('/get_details', function (req,res,next) {
    var data = req.body;

    var partner_port = data.port_of_this_tpm;
    var partner_address = data.address_of_this_tpm;
    var partner_name = data.service_name;
    //console.log(data);
    //find empty service
    var aproporiate_service;
    Object.keys(services).forEach(function(key) {
        if(services[key].partner_name == null){
            aproporiate_service = key;
        }
    });
    services[aproporiate_service].address_of_partner_tpm = partner_address;
    services[aproporiate_service].port_of_partner_tpm = partner_port;
    services[aproporiate_service].partner_name =  partner_name;
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
            res.json({service_name: services[aproporiate_service].service_name, address_of_this_tpm: services[aproporiate_service].address_of_this_tpm, port_of_this_tpm: services[aproporiate_service].port_of_this_tpm});

        }
    });

});

router.get('/start_sync', function(req, res, next) {
    if(can_start_sync === false){
        res.redirect('/');
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
            res.redirect('/');
        }else{
            if(body.status === 0){
                // failed to update
                logs_at_bottom_of_screen += "<p class='log_error'>failed to start sync API /v1/options/sync</p>";

            }else{

            }
            res.redirect('/');
        }
    });
});

module.exports = router;