from flask import render_template, jsonify
from requests import get, post
import json
import urllib

from sequencer import app


@app.route("/")
@app.route("/index")
def search():
    return render_template('sequencer.html')


@app.route("/toggle/<relay_num>", methods=['POST'])
def toggle(relay_num):
    return jsonify({"new_state": post_to_sequencer('toggle', relay_num)})


@app.route("/on/<relays>", methods=['POST'])
def on(relays):
    return jsonify({"new_state": post_to_sequencer('on', relays)})


@app.route("/off/<relays>", methods=['POST'])
def off(relays):
    return jsonify({"new_state": post_to_sequencer('off', relays)})


@app.route("/get_state")
def get_state():
    return jsonify({'state': get_from_sequencer('state')})


def post_to_sequencer(function, args):
    uri = "%s/devices/%s/%s" % (
        app.config['SPARK_ENDPOINT'],
        app.config['SPARK_CORE_ID'],
        function
        )
    args = urllib.unquote(args)
    params = { 'access_token': app.config['ACCESS_TOKEN'], 'args': args }
    r = post(uri, params)
    return json.loads(r.content)['return_value']


def get_from_sequencer(variable):
    uri = "%s/devices/%s/%s" % (
        app.config['SPARK_ENDPOINT'],
        app.config['SPARK_CORE_ID'],
        variable
        )
    r = get("%s?access_token=%s" %(uri, app.config['ACCESS_TOKEN']))
    return json.loads(r.content)['result']


@app.errorhandler(404)
def page_not_found(error):
    return render_template('page_not_found.html'), 404
