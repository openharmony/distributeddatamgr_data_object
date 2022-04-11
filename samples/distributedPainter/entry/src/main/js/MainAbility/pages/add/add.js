import router from '@system.router'
import * as distr from '../../../model/DistributedDataModel.js'
export default {
    data: {
        title: "天气不错奥",
        content: "今天天气不错"
    },
    onInit() {
        console.info("objectstore in add page");
    },
    doAdd: function () {
        console.info("doAdd " + JSON.stringify(distr.g_dataModel));
        distr.g_dataModel.add(this.title, this.content);
        router.replace({
            uri: "pages/index/index",
            params: {
                dataModel: distr.g_dataModel
            }
        })
    },
    changeTitle: function (e) {
        this.title = e.text;
    },
    changeContent: function (e) {
        this.content = e.text;
    }
}
