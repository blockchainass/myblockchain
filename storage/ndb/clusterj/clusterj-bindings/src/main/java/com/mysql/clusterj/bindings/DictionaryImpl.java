/*
 *  Copyright 2010 Sun Microsystems, Inc.
 *  All rights reserved. Use is subject to license terms.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; version 2 of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA
 */

package com.myblockchain.clusterj.bindings;

import com.myblockchain.cluster.ndbj.NdbApiException;
import com.myblockchain.cluster.ndbj.NdbDictionary;
import com.myblockchain.clusterj.ClusterJDatastoreException;
import com.myblockchain.clusterj.core.store.Index;
import com.myblockchain.clusterj.core.store.Table;
import com.myblockchain.clusterj.core.util.I18NHelper;
import com.myblockchain.clusterj.core.util.Logger;
import com.myblockchain.clusterj.core.util.LoggerFactoryService;

/**
 *
 */
class DictionaryImpl implements com.myblockchain.clusterj.core.store.Dictionary {

    /** My message translator */
    static final I18NHelper local = I18NHelper
            .getInstance(DictionaryImpl.class);

    /** My logger */
    static final Logger logger = LoggerFactoryService.getFactory()
            .getInstance(DictionaryImpl.class);

    private NdbDictionary dictionary;

    public DictionaryImpl(NdbDictionary dictionary) {
        this.dictionary = dictionary;
    }

    public Index getIndex(String indexName, String tableName, String indexAlias) {
        try {
            return new IndexImpl(dictionary.getIndex(indexName, tableName), indexAlias);
        } catch (NdbApiException ndbApiException) {
            throw new ClusterJDatastoreException(local.message("ERR_Datastore"),
                    ndbApiException);
        }
    }

    public Table getTable(String tableName) {
        try {
            return new TableImpl(dictionary.getTable(tableName));
        } catch (NdbApiException ndbApiException) {
            throw new ClusterJDatastoreException(local.message("ERR_Datastore"),
                    ndbApiException);
        }
    }

}
