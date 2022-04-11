import router from '@system.router'
import * as distr from '../../../model/DistributedDataModel.js'

export default {
    data: {
        title: "天气不错奥",
        content: "今天天气不错",
        edit: "保存"
    },
    onInit() {
        console.info("objectstore in detail page");
    },
    back: function () {
        router.replace({
            uri: "pages/index/index",
            params: {
                dataModel: distr.g_dataModel
            }
        })
    },
    change: function (e) {
        this.title = e.text;
    },
    changeContent: function (e) {
        this.content = e.text;
    },
    save: function () {
        console.info("start save "+ JSON.stringify(this.data));
        distr.g_dataModel.update(this.editIndex,  this.title, this.content);
        router.replace({
            uri: "pages/index/index",
            params: {
                dataModel: distr.g_dataModel
            }
        })
    }
}
