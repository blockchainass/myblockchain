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

import com.myblockchain.cluster.ndbj.Ndb;
import com.myblockchain.cluster.ndbj.NdbApiException;
import com.myblockchain.clusterj.ClusterJDatastoreException;
import com.myblockchain.clusterj.core.store.ClusterTransaction;
import com.myblockchain.clusterj.core.util.I18NHelper;
import com.myblockchain.clusterj.core.util.Logger;
import com.myblockchain.clusterj.core.util.LoggerFactoryService;

/**
 *
 */
class DbImpl implements com.myblockchain.clusterj.core.store.Db {

    /** My message translator */
    static final I18NHelper local = I18NHelper.getInstance(DbImpl.class);

    /** My logger */
    static final Logger logger = LoggerFactoryService.getFactory()
            .getInstance(com.myblockchain.clusterj.core.store.ClusterConnection.class);

    protected Ndb ndb;

    public DbImpl(Ndb ndb) {
        this.ndb = ndb;
    }

    public void close() {
        ndb.close();
    }

    public com.myblockchain.clusterj.core.store.Dictionary getDictionary() {
        try {
            return new DictionaryImpl(ndb.getDictionary());
        } catch (NdbApiException ndbApiException) {
            throw new ClusterJDatastoreException(local.message("ERR_Datastore"),
                    ndbApiException);
        }
    }

    public ClusterTransaction startTransaction() {
        try {
            return new ClusterTransactionImpl(ndb.startTransaction());
        } catch (NdbApiException ndbApiException) {
            throw new ClusterJDatastoreException(local.message("ERR_Datastore"),
                    ndbApiException);
        }
    }

    public boolean isRetriable(ClusterJDatastoreException ex) {
        return false;
    }

}
