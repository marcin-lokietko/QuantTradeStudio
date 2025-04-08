import { Component, Inject, ViewEncapsulation } from '@angular/core';
import { MAT_SNACK_BAR_DATA } from '@angular/material/snack-bar';
import { NotificationSeverity } from './notification-severity-enum';
import { CommonModule } from '@angular/common';

@Component({
  selector: 'app-notification',
  standalone: true,
  imports: [CommonModule],
  templateUrl: './notification.component.html',
  styleUrls: ['./notification.component.scss'],
  encapsulation: ViewEncapsulation.None, //Add CSS of thos component to the global styles to style the snackbar
})
export class NotificationComponent {

  constructor(@Inject(MAT_SNACK_BAR_DATA) public data: { message: string, notificationSeverity: NotificationSeverity }) {}

  public getClasses(): any {
    return {
        isInfo: this.data.notificationSeverity == NotificationSeverity.Info,
        isError: this.data.notificationSeverity == NotificationSeverity.Error,
    }
  }
}