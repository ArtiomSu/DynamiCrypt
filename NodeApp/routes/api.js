var express  = require('express');
var request = require('request');
var router = express.Router();

var show_service_form = true;
var current_service_name = null;
var logs_at_bottom_of_screen = "";
var services = {};

router.get('/', function(req, res, next) {
    res.render('index', { current_service_name: current_service_name,  show_service_form: show_service_form, logs_at_bottom_of_screen: logs_at_bottom_of_screen });
});

router.get('/show_service_form', function(req, res, next) {
    show_service_form = true;
    console.log("button clicked ", show_service_form);
    res.redirect('/');
});



router.post('/form_get_service_name', function (req,res,next) {
    data = req.body;
    service_name = data.Service_name;
    current_service_name = service_name
    api_port = data.API_port;
    api_address = data.API_Address;
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
            logs_at_bottom_of_screen += "failed to connect to API /v1/options/init\n";
            show_service_form = true;
        }else{
            var new_service_name = body.service_name;
            current_service_name = new_service_name;
            var address_of_this_tpm = body.address_of_this_tpm;
            var port_of_this_tpm = body.port_of_this_tpm;
            services[new_service_name] = {service_name:service_name,
                address_of_this_tpm:address_of_this_tpm,
                port_of_this_tpm:port_of_this_tpm,
                address_of_partner_tpm: null,
                port_of_partner_tpm: null,
                partner_name: null
            };
        }
        res.redirect('/');
    });
});

module.exports = router;