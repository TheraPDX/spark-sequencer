=========
spark sequencer  control
=========

Intermediary step between my spark core sequencer project and end users to
protect my access_token


Synopsis
========

Spark cores can be controlled via an API if you have a valid access token.
Unfortunately an access token is currently the keys to the kingdom, so you have
to be really careful with it. This is an indirection layer between the spark
API and my end users so I don't have to give my users my access token.

Currently provides the ability to toggle the state of any individual outlet on
my relay sequencer project, turn all the outlets on, or all the outlets off.

