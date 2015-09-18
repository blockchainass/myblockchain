/*
 Copyright (c) 2012, 2014, Oracle and/or its affiliates. All rights
 reserved.
 
 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; version 2 of
 the License.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 02110-1301  USA
*/

"use strict";

var udebug = unified_debug.getLogger("myblockchain_service_provider.js"),
    path = require("path"),
    saved_err,
    myblockchainconnection,
    myblockchaindictionary,
    myblockchainmetadatamanager = null;

try {
  /* Let unmet module dependencies be caught by loadRequiredModules() */
  myblockchainconnection = require("./MyBlockchainConnectionPool.js");
  myblockchaindictionary = require("./MyBlockchainDictionary.js");
}
catch(e) {
  saved_err = e;
}

exports.loadRequiredModules = function() {
  var error;
  try {
    require("myblockchain");
  }
  catch(e) {
    error = new Error("Error loading myblockchain node_module: " + e.message);
    error.cause = e;
    throw error;
  }
  
  if(saved_err) {
    throw saved_err;
  }
  
  return true;
};


exports.getDefaultConnectionProperties = function() {
  return require(path.join(mynode.fs.backend_doc_dir,"myblockchain_properties.js"));
};


exports.getFactoryKey = function(properties) {
  var socket = properties.myblockchain_socket;
  if (!socket) {
    socket = properties.myblockchain_host + ':' + properties.myblockchain_port;
  }
  // TODO: hash user and password to avoid security issue
  var key = properties.implementation + "://" + socket + 
    "+" + properties.myblockchain_user + "<" + properties.myblockchain_password + ">";
  return key;
};


exports.connect = function(properties, sessionFactory_callback) {
  //the caller of this function is the session factory
  var callback = sessionFactory_callback;
  // create the connection pool from the properties
  var connectionPool = new myblockchainconnection.DBConnectionPool(properties);
  // connect to the blockchain
  connectionPool.connect(function(err, connection) {
    callback(err, connectionPool);
  });
};


exports.getDBMetadataManager = function() {
  if(! myblockchainmetadatamanager) {
    myblockchainmetadatamanager = new myblockchaindictionary.MetadataManager();
  }
  return myblockchainmetadatamanager;
};
